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

// global variables
char *tgt_filename = NULL;
char *tgt_extension = NULL;
char *subdir_list = NULL;
int file_count = 0;
int dir_count = 0;
off_t total_size = 0;
int found = 0;

// Call back function to list all files and directories present in path
int list_all_cb(const char *file_path,const struct stat *sb, int type_flag, struct FTW *ftwbuf){
    printf("%s\n", file_path);
    return 0;
}

// Call back function to search for files with specific name in directory
int search_file_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (strcmp(basename((char *)file_path), tgt_filename) == 0) { // strcmp comapres 2 strings lexicographically
        printf("\nFile found: %s\n", file_path); // print path of file if found
        file_count++;
    }
    return 0;
}
// Call back function to search specific folder in the  given directory and sub-directory
int list_dir_cb(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) { // Only process directories
        char fpath_copy[PATH_MAX];
        strncpy(fpath_copy, file_path, PATH_MAX - 1);
        fpath_copy[PATH_MAX - 1] = '\0';

        char *base = basename(fpath_copy); // create a base variable by taking basename of the filepath copied
        if (base && strcmp(base, subdir_list) == 0) { //checking if base is present and match with subdir
            printf("Directory found: %s\n", file_path);
            found = 1;
        }
    } else if (typeflag == FTW_DNR) { // error handling in case of file is not read
        fprintf(stderr, "Warning: Cannot read directory %s: %s\n", file_path, strerror(errno));
    } else if (typeflag == FTW_NS) { //error handling if file is not stated
        fprintf(stderr, "Warning: Cannot stat %s: %s\n", file_path, strerror(errno));
    }
    return 0;
}

// Call back function to list files with specific extension in specific directory and subdirectory
int list_by_ext_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) {
        const char *ext = strrchr(file_path, '.'); //strrchr function is used to find the last occurence of the character in string
        if (ext && strcmp(ext, tgt_extension) == 0) { //string comparison of extension and target extension
            printf("%s\n", file_path); // printing filepath
            file_count++; //increasing the count variable by 1
        }
    }
    return 0;
}

// call back function to count files
int count_fl_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) file_count++; //checking if file is present and increasing the file_count variable by 1
    return 0;
}

// call back function to count directories ansd sub directories
int count_dirs_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_D) dir_count++; // checking if directory is present and increasing dir_counter variable by 1  
    return 0;
}

// call back function to calculate total size of files
int calc_size_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) total_size += sb->st_size; // if the file type is regular, the size of file present in stat struct named sb and added with previous value in total_size variable 
    return 0;
}

// call back function to delete files by specific extension
int dlt_by_ext_cb(const char *file_path, const struct stat *sb, int type_flag, struct FTW *ftwbuf) {
    if (type_flag == FTW_F) { //  check if the file type is regular
        const char *ext = strrchr(file_path, '.'); // function used to find the last occurence of the character in string and storing it in character variable ext. 
        if (ext && strcmp(ext, tgt_extension) == 0) { // checking if the string of ext variable is same as target extension
            if (remove(file_path) == 0) { 
                printf("Deleted: %s\n", file_path); //printing after deleting the file
                file_count++;
            } else {
                perror("remove");
            }
        }
    }
    return 0;
}

// call back function to copy a directory recursively
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
        nftw(argv[2], list_all_cb, fd_limit, flags);
    } else if (strcmp(argv[1], "-srch") == 0 && argc == 4) { //sample command: dir25s -srch file_name  file_path
        tgt_filename = argv[2];
        nftw(argv[3], search_file_cb, fd_limit, flags);
        if (file_count == 0) printf("No file named '%s' found in %s\n", tgt_filename, argv[3]);
    } else if (strcmp(argv[1], "-lext") == 0 && argc == 4) { //sample command: dir25s -lext  absolute path extension
        tgt_extension = argv[3];
        nftw(argv[2], list_by_ext_cb, fd_limit, flags);
        if (file_count == 0) printf("No file with extension '%s' found in %s\n", tgt_extension, argv[2]); // print message if no file is found in the absolute path
    }else if (strcmp(argv[1], "-listdir") == 0 && argc == 4) { // sample command: dir25s -listdir  folder_name_to_search absolute path
        subdir_list = argv[2];
        if (nftw(argv[3], list_dir_cb, fd_limit, flags) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (!found) printf("No directories named '%s' found in %s\n", subdir_list, argv[3]); // message in case of no directory found
    }  else if (strcmp(argv[1], "-countf") == 0 && argc == 3) { // sample command: dir25s -countf  absolute path
        nftw(argv[2], count_fl_cb, fd_limit, flags);
        printf("Total files: %d\n", file_count); // print total number of files in directory and further subdirectories
    } else if (strcmp(argv[1], "-countd") == 0 && argc == 3) { // sample command: dir25s -countd absolute path of directory
        nftw(argv[2], count_dirs_cb, fd_limit, flags);
        printf("Total directories: %d\n", dir_count); // print total number of sub-directories present in path provided
    } else if (strcmp(argv[1], "-sizef") == 0 && argc == 3) { //sample command: dir25s -sizef absolute path
        nftw(argv[2], calc_size_cb, fd_limit, flags); 
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
