//
// Created by Anuj Puri on 2024-04-08.
//

#include <string.h>
#include <stdlib.h>

#ifndef FILE_DOWNLOAD_SERVER_MEM_UTIL_H
#define FILE_DOWNLOAD_SERVER_MEM_UTIL_H

#endif //FILE_DOWNLOAD_SERVER_MEM_UTIL_H

void free_array(void** arr);


void free_array(void** arr) {
    if(arr == NULL) {
        return;
    }

    for(int i=0; arr[i] != NULL; i++) {
        free(arr[i]);
    }

    free(arr);
}