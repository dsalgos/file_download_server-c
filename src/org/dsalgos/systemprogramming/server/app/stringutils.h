//
// Created by Anuj Puri on 2024-03-28.
//

#ifndef FILE_DOWNLOAD_SERVER_STRINNGUTILS_H
#define FILE_DOWNLOAD_SERVER_STRINNGUTILS_H

#define MAX_STR_TOKENS 20

#endif //FILE_DOWNLOAD_SERVER_STRINNGUTILS_H


#include <string.h>

//constant literals
const char C_SPACE = ' ';
const char C_PERIOD = '.';
const char C_NEW_LINE = '\n';
const char C_NULL = '\0';
const char C_TILDA = '~';

const char* SYMBOL_FWD_SLASH = "/";
const char* STR_SPACE = " ";
//string returns
char* trim(const char* str);
int extcmp(const char* f_name, char* ext);


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


/**
 * Check if the file contains the required extension.
 * @param f_name name of the file
 * @param ext extension to be compared.
 * @return 0 if the files as extenstion @ext, else > 0, and -1 in case any of the arg is NULL.
 */
int extcmp(const char* f_name, char* ext) {

    if(f_name == NULL || ext == NULL) {
        return -1;
    }

    size_t filename_len = strlen(f_name);

    // Ensure filename is at least 5 characters long (including ".txt")
    if (filename_len < 5) {
        return -1;  // Not a .txt file
    }

    size_t idx_period = strlen(f_name)-1;
    for(;idx_period >= 0; idx_period--) {
        if(f_name[idx_period] == C_PERIOD) {
            break;
        }
    }
    //comparing the characters starting from idx_period, include period symbol.
    return strcmp(f_name + idx_period, ext);
}

/**
 * Tokenize the provided string using delimiter.
 * The function does not modify the existing "str" provided.
 * @param str string to be tokenized.
 * @param delim delimiter value.
 * @return a vector of tokens, each row contains one token.
 */
char** tokenize(char* str, char delim, int* count) {

    char* temp = malloc(sizeof(char) * strlen(str)+1);
    strcpy(temp, str);

    char* token = NULL;
    token = strtok(temp, &delim);
    int num_tokens = 0;
    while(token != NULL) {
        num_tokens++;
        token = strtok(NULL,  &delim);
    }
    *count = num_tokens;
    char** tokens = NULL;
    tokens = malloc(sizeof(char *) * num_tokens);

    temp = malloc(sizeof(char) * strlen(str)+1);
    strcpy(temp, str);
    token = strtok(temp, &delim);
    printf(" toke is %s\n", token);
    num_tokens = 0;
    while(token != NULL) {
        tokens[num_tokens] = malloc(strlen(token)+1);
        strcpy(tokens[num_tokens++], token);
        token = strtok(NULL,  &delim);
    }

    return tokens;
}

