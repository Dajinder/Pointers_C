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


char *target_filename = NULL;
char *target_extension = NULL;
int file_count = 0;
int dir_count = 0;
off_t total_size = 0;

// Function to list all files and directories
int list_all(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    printf("%s\n", fpath);
    return 0;
}

// Function to search for files with specific name
int search_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (strcmp(basename((char *)fpath), target_filename) == 0) {
        printf("%s\n", fpath);
        file_count++;
    }
    return 0;
}

// Function to list files with specific extension
int list_by_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        const char *ext = strrchr(fpath, '.');
        if (ext && strcmp(ext, target_extension) == 0) {
            printf("%s\n", fpath);
            file_count++;
        }
    }
    return 0;
}

// Function to count files
int count_files(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) file_count++;
    return 0;
}

// Function to count directories
int count_dirs(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) dir_count++;
    return 0;
}

// Function to calculate total size of files
int calc_size(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) total_size += sb->st_size;
    return 0;
}

// Function to delete files by extension
int delete_by_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        const char *ext = strrchr(fpath, '.');
        if (ext && strcmp(ext, target_extension) == 0) {
            if (remove(fpath) == 0) {
                printf("Deleted: %s\n", fpath);
                file_count++;
            } else {
                perror("remove");
            }
        }
    }
    return 0;
}

// Function to copy a directory recursively
void copy_dir(const char *src, const char *dest) {
    struct stat st;
    if (stat(src, &st) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // Creation of destination directory
    if (mkdir(dest) == -1 && errno != EEXIST) {
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
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(src_path, PATH_MAX, "%s/%s", src, entry->d_name);
        snprintf(dest_path, PATH_MAX, "%s/%s", dest, entry->d_name);

        if (stat(src_path, &st) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            copy_dir(src_path, dest_path);
        } else if (S_ISREG(st.st_mode)) {
            FILE *src_file = fopen(src_path, "rb");
            FILE *dest_file = fopen(dest_path, "wb");
            if (!src_file || !dest_file) {
                perror("fopen");
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

// Function to move a directory from one location to another
void move_dir(const char *src, const char *dest) {
    if (rename(src, dest) == -1) {
        perror("rename");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <option> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int flags = FTW_PHYS;
    int fd_limit = 20;

    if (strcmp(argv[1], "-list") == 0 && argc == 3) {
        nftw(argv[2], list_all, fd_limit, flags);
    } else if (strcmp(argv[1], "-srch") == 0 && argc == 4) {
        target_filename = argv[2];
        nftw(argv[3], search_file, fd_limit, flags);
        if (file_count == 0) printf("No file named '%s' found in %s\n", target_filename, argv[3]);
    } else if (strcmp(argv[1], "-lext") == 0 && argc == 4) {
        target_extension = argv[3];
        nftw(argv[2], list_by_extension, fd_limit, flags);
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", target_extension, argv[2]);
    } else if (strcmp(argv[1], "-countf") == 0 && argc == 3) {
        nftw(argv[2], count_files, fd_limit, flags);
        printf("Total files: %d\n", file_count);
    } else if (strcmp(argv[1], "-countd") == 0 && argc == 3) {
        nftw(argv[2], count_dirs, fd_limit, flags);
        printf("Total directories: %d\n", dir_count);
    } else if (strcmp(argv[1], "-sizef") == 0 && argc == 3) {
        nftw(argv[2], calc_size, fd_limit, flags);
        printf("Total size of files: %jd bytes\n", (intmax_t)total_size);
    } else if (strcmp(argv[1], "-cp") == 0 && argc == 4) {
        copy_dir(argv[2], argv[3]);
        printf("Copied %s to %s\n", argv[2], argv[3]);
    } else if (strcmp(argv[1], "-mv") == 0 && argc == 4) {
        move_dir(argv[2], argv[3]);
        printf("Moved %s to %s\n", argv[2], argv[3]);
    } else if (strcmp(argv[1], "-dlt") == 0 && argc == 4) {
        target_extension = argv[3];
        nftw(argv[2], delete_by_extension, fd_limit, flags);
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", target_extension, argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
