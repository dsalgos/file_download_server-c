//
// Created by Anuj Puri on 2024-03-28.
//
#ifndef FILE_DOWNLOAD_SERVER_FILEUTIL_H
#define FILE_DOWNLOAD_SERVER_FILEUTIL_H
#define MAX_BUFFER_FILE_SIZE 256
#define MAX_BUFFER_NAME 128

//user defined directives
#define CODE_ERROR_APP  (-99)
#define CODE_FILE_FOUND 999
#define F_OPEN_LIMIT 100
#define MAX_BUFFER_SIZE 512
#endif //FILE_DOWNLOAD_SERVER_FILEUTIL_H

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>
#include <time.h>
#include <sys/syslimits.h>

// USER INCLUDES
#include "dentry.h"
#include "fdetails.h"
#include "stringutils.h"


//directive required for nftw() function
#define _XOPEN_SOURCE 600


//declaring global variables

//constant global variables
const char* msg_file_not_found = "Search Unsuccessful.";

//global variables
char *f_path_tracker = NULL;
char *d_storg_path = NULL;
char* user_input_file_name =  NULL;
char* usr_f_extension = NULL;
struct fdetails* fs_details;

//integer variables.
int found_file;
int* min_f_sz = NULL;
int* max_f_sz = NULL;
unsigned long ufs_ctime = 0;

//integer returning functions.
int copy_file_fd(int fd_src, int fd_dest);
void copy_file(const char* src_file, const char* dest_file);
int open_file(const char* f_path, int oargs);
int list_dir_sort(char *d_path, struct dentry** list, int (*sort_compare)(const void*, const void*));
int f_callback_bsize(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw);
int f_callback_name(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw);
int f_callback_dt(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw);
int f_callback_extsn(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw);


//string returning functions.
char* expand_if_tilda(char *command);


//function pointer
void (*fn_cp_handle)(const char* , const char*);

int (*f_cdate_cmp)(unsigned long fs_ctime, unsigned long ufs_ctime);

//string literals
const char* cmd_tar = "tar -cf ";
const char* name_tar = "a1.tar";
const char c_extsn = '.'; // to find the file extension

/**
 * This function implements the requirement of searching a given file name inside the
 * provided path, recursively. It uses the standard library function nftw(), which
 * does the recursive walk.
 * @param root_path
 * @param file_name
 * @return
 */
struct fdetails* file_search(const char* root_path, const char* file_name, int (*f_callback)(const char* , const struct stat*, int, struct FTW*)) {

    //let's free up the memory to make sure there are no leaks.
    //don't want to take the risk of freeing up the same memory twice.
    //it's not recommended at all as it can lead to unidentified behavior.
    if(f_path_tracker != NULL) {
        free(f_path_tracker);
    }

    //allocating new memory, we definitely need it to save the path
    //I mean absolute path of the file.
    //the responsibility of free-ing up this memory lies on the calling method
    //as this reference is returned.
    f_path_tracker = malloc(sizeof(char) * PATH_MAX);

    //hate the null checks but need it...
    if(root_path == NULL || f_callback == NULL || file_name == NULL) {
        perror("invalid arguments supplied, operation failed.");
        exit(CODE_ERROR_APP);
    }

    //tracking the user
    fs_details = malloc(sizeof(f_details_entry));
    user_input_file_name = file_name;
    int code = nftw(root_path, f_callback, F_OPEN_LIMIT, FTW_PHYS);
    if(code == -1) {
        printf("\n%s ", msg_file_not_found);
        exit(CODE_ERROR_APP);
    }

    if(found_file != 1) {
        printf("\n%s ", msg_file_not_found);
    }

    if(code == CODE_FILE_FOUND) {
        printf("%s", " Search successful. ");
        return fs_details;
    } else {
        free(f_path_tracker); //in case of returning NULL, let's free up the memory.
        return NULL;
    }
}


/**
 * function to handle the callback from "not file tree walk" nftw()
 * this shall be called everytime a directory or file is encountered.
 * The function will handle callbacks only for type "file" or F_FTW
 * @param path_current
 * @param f_stat
 * @param f_type
 * @param ptr_struct_ftw
 * @return
 */
int f_callback_name(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw) {
    if(f_type == FTW_F) {

        //use the base to find the current file name, add base to f_current address
        //FTW is a structure, where base is a member variable
        //base contains the location of the base file_name
        //this is just a position, when you add base to path_current
        //basically you are doing a pointer arithmetic operation
        //and then f_current contains the pointer to location
        //in a char* where the file_name starts.
        const char *f_current = path_current + ptr_struct_ftw->base;

        if (user_input_file_name != NULL && f_current != NULL && strcmp(f_current, user_input_file_name) == 0) {
            //once the file is found, returning non-zero value to stop the traversal.
            realpath(path_current, f_path_tracker);

            //enabling a boolean as the callback will return to nftw() only
            //though nftw will also return the same code but need to
            // return some other value from the caller function.
            if(fs_details != NULL) {
                fs_details->f_name = user_input_file_name;
                fs_details->f_size = f_stat->st_size;
                fs_details->f_mode = get_permissions(f_stat->st_mode);
            }
            found_file = 1;
            printf("\n%s %s ", "Search successful. Absolute path is - ", f_path_tracker);
            return CODE_FILE_FOUND;
        }
    }

    //Returning '0' so that nftw() contrinues the traversal to find the file.
    return 0;
}

/**
 * function to handle the callback from "not file tree walk" nftw()
 * this shall be called everytime a directory or file is encountered.
 * The function will handle callbacks only for type "file" or F_FTW
 * @param path_current
 * @param f_stat
 * @param f_type
 * @param ptr_struct_ftw
 * @return
 */
int f_callback_extsn(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw) {
    if(f_type == FTW_F) {

        //use the base to find the current file name, add base to f_current address
        //FTW is a structure, where base is a member variable
        //base contains the location of the base file_name
        //this is just a position, when you add base to path_current
        //basically you are doing a pointer arithmetic operation
        //and then f_current contains the pointer to location
        //in a char* where the file_name starts.
        const char *f_current = path_current + ptr_struct_ftw->base;
        const char* f_extension = strchr(path_current, c_extsn);

       if(usr_f_extension != NULL && f_extension != NULL && (strcmp(f_extension, usr_f_extension) == 0)) {
            realpath(path_current, f_path_tracker);
            printf("\n%s %s ", "Search successful. Absolute path is - ", f_path_tracker);

            //if this function pointer is not NULL, that means the call is made to nftw() via
            //f_extension_search, which indeed requires to not just find the files with provided extension
            //but also process them to create a .tar file
            //this function will help to copy all the files found as per extension match to a
            //storage directory
            if(fn_cp_handle != NULL && d_storg_path != NULL) {
                strcat(d_storg_path, "/");
                unsigned long pos_slash = strlen(d_storg_path) -1;

                strcat(d_storg_path, f_current);
                printf("\n storage path : %s\n", d_storg_path);
                fn_cp_handle(f_path_tracker, d_storg_path);

                //putting a logic to reuse the existing string memory.
                recycle_str(d_storg_path, pos_slash);
                printf(" \nresuming storage path : %s\n", d_storg_path);
            }

            //not returning anything here. The search must be continued for all the files.
            //in case of extension file search.

        }
    }

    //Returning '0' so that nftw() contrinues the traversal to find the file.
    return 0;
}


/**
 * function to handle the callback from "not file tree walk" nftw()
 * this shall be called everytime a directory or file is encountered.
 * The function will handle callbacks only for type "file" or F_FTW
 * and only record the files which have specific size limits.
 * @param path_current path to the current file.
 * @param f_stat stat variable for the file
 * @param f_type type of the file
 * @param ptr_struct_ftw
 * @return
 */
int f_callback_bsize(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw) {
    if(f_type == FTW_F) {

        //use the base to find the current file name, add base to f_current address
        //FTW is a structure, where base is a member variable
        //base contains the location of the base file_name
        //this is just a position, when you add base to path_current
        //basically you are doing a pointer arithmetic operation
        //and then f_current contains the pointer to location
        //in a char* where the file_name starts.
        const char *f_current = path_current + ptr_struct_ftw->base;

        if(usr_f_extension == NULL || min_f_sz == NULL || max_f_sz == NULL) {
            return 1;
        }

        if((f_stat->st_blksize >= *min_f_sz && f_stat->st_blksize <= *max_f_sz)) {
            realpath(path_current, f_path_tracker);
            printf("\n%s %s ", "Search successful. Absolute path is - ", f_path_tracker);

            //if this function pointer is not NULL, that means the call is made to nftw() via
            //f_extension_search, which indeed requires to not just find the files with provided extension
            //but also process them to create a .tar file
            //this function will help to copy all the files found as per extension match to a
            //storage directory
            if(fn_cp_handle != NULL && d_storg_path != NULL) {
                strcat(d_storg_path, SYMBOL_FWD_SLASH);
                unsigned long pos_slash = strlen(d_storg_path) -1;

                strcat(d_storg_path, f_current);
                printf("\n storage path : %s\n", d_storg_path);
                fn_cp_handle(f_path_tracker, d_storg_path);

                //putting a logic to reuse the existing string memory.
                recycle_str(d_storg_path, pos_slash);
                printf(" \nresuming storage path : %s\n", d_storg_path);
            }

            //not returning anything here. The search must be continued for all the files.
            //in case of extension file search.

        }
    }

    //Returning '0' so that nftw() contrinues the traversal to find the file.
    return 0;
}

int f_callback_dt(const char *path_current, const struct stat *f_stat, int f_type, struct FTW *ptr_struct_ftw) {
    if(f_type == FTW_F) {

        //use the base to find the current file name, add base to f_current address
        //FTW is a structure, where base is a member variable
        //base contains the location of the base file_name
        //this is just a position, when you add base to path_current
        //basically you are doing a pointer arithmetic operation
        //and then f_current contains the pointer to location
        //in a char* where the file_name starts.
        const char *f_current = path_current + ptr_struct_ftw->base;

        if(usr_f_extension == NULL || f_cdate_cmp == NULL) {
            return 1;
        }

        if(f_cdate_cmp(f_stat->st_ctimespec.tv_nsec, ufs_ctime)) {
            realpath(path_current, f_path_tracker);
            printf("\n%s %s ", "Search successful. Absolute path is - ", f_path_tracker);

            //if this function pointer is not NULL, that means the call is made to nftw() via
            //f_extension_search, which indeed requires to not just find the files with provided extension
            //but also process them to create a .tar file
            //this function will help to copy all the files found as per extension match to a
            //storage directory
            if(fn_cp_handle != NULL && d_storg_path != NULL) {
                strcat(d_storg_path, SYMBOL_FWD_SLASH);
                unsigned long pos_slash = strlen(d_storg_path) -1;

                strcat(d_storg_path, f_current);
                printf("\n storage path : %s\n", d_storg_path);
                fn_cp_handle(f_path_tracker, d_storg_path);

                //putting a logic to reuse the existing string memory.
                recycle_str(d_storg_path, pos_slash);
                printf(" \nresuming storage path : %s\n", d_storg_path);
            }

            //not returning anything here. The search must be continued for all the files.
            //in case of extension file search.

        }
    }

    //Returning '0' so that nftw() contrinues the traversal to find the file.
    return 0;
}

/*
 *  A utility function to copy file from one location to
 *  another location.
 */
void copy_file(const char* src_file, const char* dest_file) {

    printf("\n source file %s, and destination file %s", src_file, dest_file);

    int fd_src = open_file(src_file, O_RDONLY);
    int fd_dest = open_file(dest_file, O_RDWR);

    //in case of any error, report and exit.
    if(fd_src == -1) {
        printf("\n%s", "file copying failed. source file does not exist.");
        exit(CODE_ERROR_APP);
    } else if(fd_dest == -1 ) {
        fd_dest = open_file(dest_file, O_CREAT|O_RDWR);
        if(fd_dest == -1) {
            printf("\n%s", "file copying failed. cannot create destination file.");
            exit(CODE_ERROR_APP);
        }
    }

    //now, let's start the process of reading the data from the src file
    copy_file_fd(fd_src, fd_dest);
}

//print absolute path using nftw(), and callback() functions
void f_extension_search(const char* root_path, const char* storage_path, char* f_extension) {

    //opening these fds just to make sure the paths exist
    //don't want to call nftw() with NULL values
    //though it may handle, but i don't know the internal implementation of nftw()
    //so not relying on it.
    int fd_root = open_file(root_path, O_RDONLY);
    printf(" fd_root %d, root_path %s\n", fd_root, root_path);
    if(fd_root == -1) {
        perror(" f_extension_search: root path not found.");
        exit(CODE_ERROR_APP);
    }

    if(NULL == storage_path) {
        perror("error occurred");
        exit(CODE_ERROR_APP);
    }

    //allocating dynamic memory for managing the destination path for
    //copying or moving the file.
    char * f_dest_path = malloc(sizeof(char)*(strlen(storage_path)));
    realpath(storage_path, f_dest_path);

    struct stat status_block;

    if(stat(f_dest_path, &status_block) == 0 && S_ISDIR(status_block.st_mode)) {
        printf("%s ", f_dest_path);
        //DO NOTHING WE JUST NEED TO CREATE STORAGE PATH
    } else {
        if(mkdir(f_dest_path, 0777) == 0) {
            //DO NOTHING WE JUST NEED TO CREATE STORAGE PATH
        } else {
            perror(" f_extension_search: storage path not found.");
            exit(CODE_ERROR_APP);
        }
    }

    //allocating new memory, we definitely need it to save the path
    //I mean absolute path of the file.
    //the responsibility of free-ing up this memory lies on the calling method
    //as this reference is returned.
    f_path_tracker = malloc(sizeof(char) * PATH_MAX);

    fn_cp_handle = &copy_file;
    usr_f_extension = f_extension;
    d_storg_path = f_dest_path;
    int code = nftw(root_path, f_callback_extsn, F_OPEN_LIMIT, FTW_PHYS);
    if(code == -1) {
        printf("\n%s ", msg_file_not_found);
        exit(CODE_ERROR_APP);
    }

    printf("\n\ndone with processing extension files.....\n");
    char* cmd_tar_create = malloc(sizeof(char)*128);

    //building the tar command.
    strcat(cmd_tar_create, cmd_tar);
    strcat(cmd_tar_create, name_tar);
    strcat(cmd_tar_create, " ");
    strcat(cmd_tar_create, d_storg_path);
    system(cmd_tar_create);

    //release the resources.
    free(f_dest_path);
    free(f_path_tracker);
    free(cmd_tar_create);
    close(fd_root);
}

/*
 *  A utility function to copy file from one location to
 *  another location. The file descriptors are not closed
 *  in this function, and the responsibility lies on the
 *  calling method.
 */
int copy_file_fd(const int fd_src, const int fd_dest) {


    //creating a buffer, of size MAX_BUFFER_FILE_SIZE
    char* buffer = malloc(sizeof(char) * MAX_BUFFER_FILE_SIZE);

    //now, let's start the process of reading the data from the src file
    //read until there is nothing to be read or written.
    while(1) {
        long int rb = read(fd_src, buffer, MAX_BUFFER_FILE_SIZE);
        if(rb <=0) break;

        long int wb = write(fd_dest, buffer, MAX_BUFFER_FILE_SIZE);
        if(wb <= 0) break;
    }

    close(fd_src);
    close(fd_dest);
    free(buffer);
    return 0;
}

//open the file
int open_file(const char* f_path, const int oargs) {
    if(f_path == NULL) {
        printf("\n%s", " file path cannot be null.");
        return -1;
    }

    return open(f_path, oargs, 0777);
}

/**
 * Comparator to find the relatively larger timestamp.
 * @param x_time
 * @param y_time
 * @return
 */
int d_compare_modified(const void *x_time, const void *y_time) {
    const struct dentry *entry1 = (const struct dentry *)x_time;
    const struct dentry *entry2 = (const struct dentry *)y_time;

    return difftime(entry1->stat.st_mtimespec.tv_nsec, entry2->stat.st_mtimespec.tv_nsec);
}

/**
 * Comparator the names of the directories.
 * @param entry1
 * @param
 * @return
 */
int d_compare_name(const void *entry1, const void *entry2) {
    const struct dentry *d_entry1 = (const struct dentry *)entry1;
    const struct dentry *d_entry2 = (const struct dentry *)entry2;

    return strcmp(d_entry1->f_name, d_entry2->f_name);
}

DIR* open_dir(const char *d_path) {
    DIR  *_d = opendir(d_path);
    if(_d == NULL) {
        perror("error opening directory");
    }

    return _d;
}

int is_linux() {
    struct utsname buffer;

    if (uname(&buffer) == -1) {
        perror("uname");
        return 1;
    }

    if (strcmp(buffer.sysname, "Darwin") == 0) {
        printf("This is a macOS system.\n");
        return 0;
    } else {
        return 1;
    }
}

/**
 * Utility function to substitute  path for home directory
 * represented by '~'.
 * @param command
 * @return
 */
char* expand_if_tilda(char *command) {

    if(command == NULL) {
        return NULL;
    }

    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("getenv");
        exit(1);
    }

    size_t size = strlen(command) + strlen(home_dir)+1;
    char *exp_command = malloc(sizeof(char)*size);

    if(exp_command == NULL) {
        perror("malloc");
        exit(1);
    }

    size_t i = 0, j = 0;
    for(; command[i] != '\0'; i++, j++) {
        if(command[i] == C_TILDA) {
            strcpy(exp_command+j, home_dir);
            j += strlen(home_dir);
        } else {
            exp_command[j] = command[i];
        }
    }

    exp_command[i] = C_NULL;
    return exp_command;
}

/**
 * Implementation of "dirlist" command, to get teh list of files from the directory @d_path.
 * The list is sorted based on the comparator pased as @sort_compare, using qsort library function.
 * @param d_path path to the root directory, where the
 * @param list the final list which contains list of names of subdirectories as part of @d_path
 * @param sort_compare pointer the function used for sorting the elements in the list
 * @return
 */
int list_dir_sort(char* d_path, struct dentry** list, int (*sort_compare)(const void*, const void*)) {

    if(sort_compare == NULL) {
        sort_compare = (int (*)(const void *, const void *))strcmp;
    }

    //try to open the directory entry
    DIR *_d = open_dir(d_path);

    // check if the _d directory is opened or not.
    if(_d != NULL) {
        struct stat e_stat;
        struct dirent *ptr_file;
        int i_count=0;
        //skip the local dir and parent directory.
        while((ptr_file = readdir(_d)) != NULL
              && !(strcmp(ptr_file->d_name, ".") ==0
                   || strcmp(ptr_file->d_name, "..") ==0)) {

            stat(ptr_file->d_name, &e_stat);
            if(S_ISDIR(e_stat.st_mode)) {
                list[i_count] = malloc(sizeof (d_entry_details)); //+1 for '\0' char character
                if(list[i_count] != NULL) {
                    strcpy(list[i_count]->f_name, ptr_file->d_name);
                    list[i_count]->stat = e_stat;
                    ++i_count;
                }
            }
        }

        //now as we have the list of all the directories present in the home directory
        //we need to sort the list by directory name in alphabetical order
        qsort(*list, i_count, sizeof(char *), sort_compare);
        return i_count;
    }

    return 0;
}