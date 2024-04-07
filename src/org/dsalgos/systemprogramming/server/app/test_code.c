//
// Created by Anuj Puri on 2024-04-06.
//
#include <stdio.h>
#include <dirent.h>

DIR* open_dir(const char *d_path) {
    DIR  *_d = opendir(d_path);
    if(_d == NULL) {
        perror("error opening directory");
    }

    return _d;
}


int main() {

    DIR  *dir = open_dir(".");

    if(dir == NULL) {
        perror("opening dir failed.\n");
    } else {
        printf(" directory opened is --> %s\n", readdir(dir)->d_name);
    }
}