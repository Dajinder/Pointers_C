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


char *tgt_filename = NULL;
char *tgt_extension = NULL;
char *subdir_list = NULL;
int file_count = 0;
int dir_count = 0;
off_t total_size = 0;
int found = 0;

// Function to list all files and directories
int list_all(const char *file_path){
    printf("%s\n", file_path);
    return 0;
}

// Function to search for files with specific name
int search_file(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (strcmp(basename((char *)file_path), tgt_filename) == 0) { // strcmp comapres 2 strings lexicographically
        printf("\nFile found: %s\n", file_path); // print path of file if found
        file_count++;
    }
    return 0;
}

int listdir_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) { // Only process directories
        // Defensive copy to avoid modifying original path
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
int list_by_extension(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) {
        const char *ext = strrchr(file_path, '.'); //strrchr function is used to find the last occurence of the character in string
        if (ext && strcmp(ext, tgt_extension) == 0) {
            printf("%s\n", file_path);
            file_count++;
        }
    }
    return 0;
}

// Function to count files
int count_files(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) file_count++;
    return 0;
}

// Function to count directories
int count_dirs(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_D) dir_count++;
    return 0;
}

// Function to calculate total size of files
int calc_size(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) total_size += sb->st_size;
    return 0;
}

// Function to delete files by extension
int dlt_by_ext_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) {
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
            cp_dir_cb(src_path, dest_path);
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
void mv_dir_cb(const char *src, const char *dest) {
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

    if (strcmp(argv[1], "-list") == 0 && argc == 3) { // checking the second argument in argv array is -list and perform next action accordingly
        nftw(argv[2], list_all, fd_limit, flags);
    } else if (strcmp(argv[1], "-srch") == 0 && argc == 4) { //sample command: dir25s -srch file_name  file_path
        tgt_filename = argv[2];
        nftw(argv[3], search_file, fd_limit, flags);
        if (file_count == 0) printf("No file named '%s' found in %s\n", tgt_filename, argv[3]);
    } else if (strcmp(argv[1], "-lext") == 0 && argc == 4) { //sample command: dir25s -lext  absolute path extension
        tgt_extension = argv[3];
        nftw(argv[2], list_by_extension, fd_limit, flags);
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", tgt_extension, argv[2]); // print message if no file is found in the absolute path
    } else if (strcmp(argv[1], "-listdir") == 0 && argc == 4) {
        subdir_list = argv[2];
        if (nftw(argv[3], listdir_callback, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (!found) printf("No directories named '%s' found in %s\n", subdir_list, argv[3]);
    } else if (strcmp(argv[1], "-countf") == 0 && argc == 3) { // sample command: dir25s -countf  absolute path
        nftw(argv[2], count_files, fd_limit, flags);
        printf("Total files: %d\n", file_count); // print total number of files in directory and further subdirectories
    } else if (strcmp(argv[1], "-countd") == 0 && argc == 3) { // sample command: dir25s -countd absolute path of directory
        nftw(argv[2], count_dirs, fd_limit, flags);
        printf("Total directories: %d\n", dir_count); // print total number of sub-directories present in path provided
    } else if (strcmp(argv[1], "-sizef") == 0 && argc == 3) { //sample command: dir25s -sizef absolute path
        nftw(argv[2], calc_size, fd_limit, flags); 
        printf("Total size of files: %jd bytes\n", (intmax_t)total_size); // prints total bytes of files present in the provided path
    } else if (strcmp(argv[1], "-cp") == 0 && argc == 4) { // sample commannd: dir25s -cp (absolute path of directory and sub directory to be copied) (name of directory for content to be copied)
        cp_dir_cb(argv[2], argv[3]);
        printf("Copied %s to %s\n", argv[2], argv[3]); //printing statement after successful copy creation
    } else if (strcmp(argv[1], "-mv") == 0 && argc == 4) { // sample command: dir25s -mv path of source directory(backup) path of destination directory(backup_1)
        mv_dir_cb(argv[2], argv[3]);
        printf("Moved %s to %s\n", argv[2], argv[3]); 
    } else if (strcmp(argv[1], "-dlt") == 0 && argc == 4) {
        tgt_extension = argv[3];
        nftw(argv[2], dlt_by_ext_cb, fd_limit, flags);
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", tgt_extension, argv[2]);
    } else {
        fprintf(stderr, "Invalid arguments.\n");// printing error in case of any of the above operation is not performed properly
        exit(EXIT_FAILURE);
    }

    return 0;
}
