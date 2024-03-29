//
// Created by Anuj Puri on 2024-03-28.
//

#ifndef FILE_DOWNLOAD_SERVER_STRINNGUTILS_H
#define FILE_DOWNLOAD_SERVER_STRINNGUTILS_H

#endif //FILE_DOWNLOAD_SERVER_STRINNGUTILS_H

#include <string.h>

//string literals
const char C_SPACE = ' ';

//string returns
char* trim(const char* str);

/**
 * Utility function to remove the leading and trailing space
 * in the provided string.
 * @param str string
 * @return
 */
char* trim(const char* str) {
    char* begin_str = str;
    if(*begin_str == '\0') {
        return begin_str;
    }

    //find the first non-space character,
    while(*begin_str == C_SPACE) {
        begin_str++;
    }

    //if found null, return str
    if(*begin_str == '\0') {
        return begin_str;
    }

    char* from_end = begin_str + strlen(begin_str) - 1;
    while(from_end > begin_str && *from_end == C_SPACE) {
        from_end--;
    }

    *(from_end+1) = '\0';

    return begin_str;
}

