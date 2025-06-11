#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <libgen.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <inttypes.h>

#ifdef _WIN32
#include <direct.h> // For _mkdir on Windows
#define mkdir(path, mode) _mkdir(path) // Map POSIX mkdir to Windows _mkdir
#endif

char *tgt_filename = NULL;
char *tgt_extension = NULL;
char *subdir_list = NULL;
int file_count = 0;
int dir_count = 0;
off_t total_size = 0;
int found = 0;

// Function to list all files and directories
int list_all(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    printf("%s\n", file_path);
    return 0;
}

// Function to search for files with specific name
int search_file(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) { // Only process regular files
        char *base = basename((char *)file_path);
        if (base && strcmp(base, tgt_filename) == 0) {
            printf("File found: %s\n", file_path);
            file_count++;
        }
    }
    return 0;
}

// Function to list directories with specific name
int listdir_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) { // Only process directories
        char fpath_copy[PATH_MAX];
        strncpy(fpath_copy, fpath, PATH_MAX - 1);
        fpath_copy[PATH_MAX - 1] = '\0';

        char *base = basename(fpath_copy);
        if (base && strcmp(base, subdir_list) == 0) {
            printf("Directory found: %s\n", fpath);
            found = 1;
        }
    } else if (typeflag == FTW_DNR) {
        fprintf(stderr, "Warning: Cannot read directory %s: %s\n", fpath, strerror(errno));
    } else if (typeflag == FTW_NS) {
        fprintf(stderr, "Warning: Cannot stat %s: %s\n", fpath, strerror(errno));
    }
    return 0;
}

// Function to list files with specific extension
int list_by_extension(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        const char *ext = strrchr(file_path, '.');
        if (ext && strcmp(ext, tgt_extension) == 0) {
            printf("%s\n", file_path);
            file_count++;
        }
    }
    return 0;
}

// Function to count files
int count_files(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) file_count++;
    return 0;
}

// Function to count directories
int count_dirs(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) dir_count++;
    return 0;
}

// Function to calculate total size of files
int calc_size(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) total_size += sb->st_size;
    return 0;
}

// Function to delete files by extension
int dlt_by_ext_cb(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        const char *ext = strrchr(file_path, '.');
        if (ext && strcmp(ext, tgt_extension) == 0) {
            if (remove(file_path) == 0) {
                printf("Deleted: %s\n", file_path);
                file_count++;
            } else {
                perror("remove");
            }
        }
    }
    return 0;
}

// Function to copy a directory recursively
void cp_dir_cb(const char *src, const char *dest) {
    struct stat st;
    if (stat(src, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // Create destination directory
    if (mkdir(dest, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(src);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
        snprintf(dest_path, PATH_MAX, "%s/%s", dest, entry->d_name);

        if (stat(src_path, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            cp_dir_cb(src_path, dest_path);
        } else if (S_ISREG(st.st_mode)) {
            FILE *src_file = fopen(src_path, "rb");
            FILE *dest_file = fopen(dest_path, "wb");
            if (!src_file || !dest_file) {
                perror("fopen");
                if (src_file) fclose(src_file);
                if (dest_file) fclose(dest_file);
                continue;
            }
            char buf[8192];
            size_t n;
            while ((n = fread(buf, 1, sizeof(buf), src_file)) > 0) {
                fwrite(buf, 1, n, dest_file);
            }
            fclose(src_file);
            fclose(dest_file);
        }
    }
    closedir(dir);
}

// Function to move a directory
void mv_dir_cb(const char *src, const char *dest) {
    if (rename(src, dest) == -1) {
        perror("rename");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <option> [args...]\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -list <path>                List all files and directories\n");
        fprintf(stderr, "  -srch <filename> <path>     Search for a specific file\n");
        fprintf(stderr, "  -lext <path> <extension>    List files with specific extension\n");
        fprintf(stderr, "  -listdir <dirname> <path>   List directories with specific name\n");
        fprintf(stderr, "  -countf <path>              Count total files\n");
        fprintf(stderr, "  -countd <path>              Count total directories\n");
        fprintf(stderr, "  -sizef <path>               Calculate total file size\n");
        fprintf(stderr, "  -cp <src> <dest>            Copy directory recursively\n");
        fprintf(stderr, "  -mv <src> <dest>            Move directory\n");
        fprintf(stderr, "  -dlt <path> <extension>     Delete files with specific extension\n");
        exit(EXIT_FAILURE);
    }

    // Reset global counters before each operation
    file_count = 0;
    dir_count = 0;
    total_size = 0;
    found = 0;

    int flags = FTW_PHYS;
    int fd_limit = 20;

    if (strcmp(argv[1], "-list") == 0 && argc == 3) {
        if (nftw(argv[2], list_all, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[1], "-srch") == 0 && argc == 4) {
        tgt_filename = argv[2];
        if (nftw(argv[3], search_file, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (file_count == 0) printf("No file named '%s' found in %s\n", tgt_filename, argv[3]);
    } else if (strcmp(argv[1], "-lext") == 0 && argc == 4) {
        tgt_extension = argv[3];
        if (nftw(argv[2], list_by_extension, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", tgt_extension, argv[2]);
    } else if (strcmp(argv[1], "-listdir") == 0 && argc == 4) {
        subdir_list = argv[2];
        if (nftw(argv[3], listdir_callback, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (!found) printf("No directories named '%s' found in %s\n", subdir_list, argv[3]);
    } else if (strcmp(argv[1], "-countf") == 0 && argc == 3) {
        if (nftw(argv[2], count_files, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printf("Total files: %d\n", file_count);
    } else if (strcmp(argv[1], "-countd") == 0 && argc == 3) {
        if (nftw(argv[2], count_dirs, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printf("Total directories: %d\n", dir_count);
    } else if (strcmp(argv[1], "-sizef") == 0 && argc == 3) {
        if (nftw(argv[2], calc_size, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        printf("Total size of files: %jd bytes\n", (intmax_t)total_size);
    } else if (strcmp(argv[1], "-cp") == 0 && argc == 4) {
        cp_dir_cb(argv[2], argv[3]);
        printf("Copied %s to %s\n", argv[2], argv[3]);
    } else if (strcmp(argv[1], "-mv") == 0 && argc == 4) {
        mv_dir_cb(argv[2], argv[3]);
        printf("Moved %s to %s\n", argv[2], argv[3]);
    } else if (strcmp(argv[1], "-dlt") == 0 && argc == 4) {
        tgt_extension = argv[3];
        if (nftw(argv[2], dlt_by_ext_cb, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", tgt_extension, argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}