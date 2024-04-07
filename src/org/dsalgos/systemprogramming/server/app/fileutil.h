//
// Created by Anuj Puri on 2024-03-28.
//

#ifndef FILE_DOWNLOAD_SERVER_FILEUTIL_H
#define FILE_DOWNLOAD_SERVER_FILEUTIL_H
#define MAX_BUFFER_FILE_SIZE 256
#define MAX_BUFFER_NAME 128

#endif //FILE_DOWNLOAD_SERVER_FILEUTIL_H

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "dentry.h"


//constant literals
const char C_NEW_LINE = '\n';
const char C_NULL = '\0';
const char C_SPACE = ' ';
const char C_TILDA = '~';

int is_txt_file(const char* f_name);
int copy_file(int fd_src, int fd_dest);
int open_file(const char* f_path, int oargs);
char* expand_if_tilda(char *command);
int list_dir_sort_by_name(char *d_path, char** list);

int is_txt_file(const char* f_name) {
    size_t filename_len = strlen(f_name);

    // Ensure filename is at least 5 characters long (including ".txt")
    if (filename_len < 5) {
        return -1;  // Not a .txt file
    }

    // Check the last 4 characters for ".txt" (case-insensitive comparison)
    return (strcmp(f_name + filename_len - 4, ".txt") == 0 ||
            strcmp(f_name + filename_len - 4, ".TXT") == 0);
}

/*
 *  A utility function to copy file from one location to
 *  another location. The file descriptors are not closed
 *  in this function, and the responsibility lies on the
 *  calling method.
 */
int copy_file(const int fd_src, const int fd_dest) {


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

int d_compare_modified(const void *x_time, const void *y_time) {
    const struct dentry *entry1 = (const struct dentry *)x_time;
    const struct dentry *entry2 = (const struct dentry *)y_time;

    return difftime(entry1->stat.st_mtimespec.tv_nsec, entry2->stat.st_mtimespec.tv_nsec);
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

char* expand_if_tilda(char *command) {

    if(command == NULL) {
        return NULL;
    }

    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("getenv");
        exit(1);
    }

    size_t size = strlen(command)+ strlen(home_dir)+1;
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
 *
 * @param d_path path to the root directory, where the
 * @param list the final list which contains list of names of subdirectories as part of @d_path
 * @param sort_compare pointer the function used for sorting the elements in the list
 * @return
 */
int list_dir_sort(char* d_path, char** list, int (*sort_compare)(const void*, const void*)) {

    if(sort_compare == NULL) {
        sort_compare = (int (*)(const void *, const void *))strcmp;
    }

    //try to open the directory entry
    DIR *_d = open_dir(d_path);
    struct stat e_stat;
    struct dirent *ptr_file;

    // check if the _d directory is opened or not.
    if(_d != NULL) {
        int i_count=0;
        //skip the local dir and parent directory.
        while((ptr_file = readdir(_d)) != NULL
              && !(strcmp(ptr_file->d_name, ".") ==0
                   || strcmp(ptr_file->d_name, "..") ==0)) {

            stat(ptr_file->d_name, &e_stat);
            if(S_ISDIR(e_stat.st_mode)) {
                list[i_count] = malloc(strlen(ptr_file->d_name)+1); //+1 for '\0' char character
                if(list[i_count] != NULL) {
                    strcpy(list[i_count], ptr_file->d_name);
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

/**
 *
 * @param d_path
 * @param list
 * @return
 */
int list_dir_sort_by_name(char *d_path, char** list) {


    return 0;
}