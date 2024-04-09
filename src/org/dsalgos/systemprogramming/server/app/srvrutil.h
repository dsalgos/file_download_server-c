//
// Created by Anuj Puri on 2024-03-25.
//

#ifndef FILE_DOWNLOAD_SERVER_SRVRUTIL_H
#define FILE_DOWNLOAD_SERVER_SRVRUTIL_H

#define MAX_BUFFER_RR_SIZE 1024 // max buffer size for reading or writing the data from the Live socket.
#define MAX_QUEUE_SIZE 50 //defining the number of request queued, before rejection triggers.
#define _XOPEN_SOURCE 500
#define PORT 8080
#define MIRROR_PORT 7001
#define RESP_BUFFER_SIZE 256
#define BUFFER_FILE_NAME 64


//char request[MAX_BUFFER_RR_SIZE];
//char response[MAX_BUFFER_RR_SIZE*2];

#endif //FILE_DOWNLOAD_SERVER_SRVRUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>

#include "fileutil.h"
#include "mem_util.h"

//command literals
char* CMD_LIST_DIR_SRTD_NAME = "dirlist -a";
char* CMD_LIST_DIR_SRTD_MTIME = "dirlist -t";
char* CMD_FILE_SRCH_NAME = "w24fs";
char* CMD_FILE_SRCH_SIZE = "w24fz";
char* CMD_FILE_SRCH_EXT = "w24ft";
char* CMD_FILE_SRCH_DATE = "w24fdb";
char* CMD_CLIENT_QUIT = "quit";

void handle_listdir_rqst(const char* command, char** reponse);

/**
 *
 * @param fd_server
 * @param address_srvr
 * @param port_local
 * @return
 */
int init_server(int *fd_server, struct sockaddr_in address_srvr);

//integer return
int is_linux();
int process_request(int fd_clnt_sckt);
int send_msg(int fd_sckt, char** msg);
int bind_address();
int listen_sckt();


int init_server(int *fd_server, struct sockaddr_in address_srvr) {

    int opt = 1; //option for server socket

    // Create socket file descriptor
    // use default protocol (i.e TCP)
    if ((*fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // error handle
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(*fd_server, SOL_SOCKET, SO_REUSEADDR /*| SO_REUSEPORT*/, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind socket to the PORT
    if (bind(*fd_server, (struct sockaddr *)&address_srvr, sizeof(address_srvr)) < 0)
    {	//error
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen_sckt to the socket
    // queue of size BACKLOG
    if (listen(*fd_server, MAX_QUEUE_SIZE) < 0)
    {
        perror("Error while listening..");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void handle_listdir_rqst(const char* command, char** response) {

    if(command == NULL) { return; }

    char* dir_home = expand_if_tilda("~");
    printf("searching find in home %s\n", dir_home);

    struct dentry** list_dentry = calloc(sizeof(struct dentry*), MAX_BUFFER_SIZE);
    int ret_value = -1;
    if(strcmp(CMD_LIST_DIR_SRTD_NAME, command) == 0) {
        ret_value = list_dir_sort(dir_home, list_dentry, d_compare_name);
    } else if(strcmp(CMD_LIST_DIR_SRTD_MTIME, command) == 0){
        ret_value = list_dir_sort(dir_home, list_dentry, d_compare_modified);
    }

    if(ret_value != -1) {
        response = malloc(sizeof(char*)  * (sizeof(list_dentry)));
        for(int i=0; list_dentry[i] != NULL; i++) {
            response[i] = calloc(sizeof(char)+2, strlen(list_dentry[i]->f_name));

            //add the file name to response
            strcpy(response[i], list_dentry[i]->f_name);
            strcpy(response[i]+ strlen(list_dentry[i]->f_name), "\n");
        }
    }

    free(dir_home);
    free(list_dentry);
    free_array((void **) response);
}

/*
 * reading request from the client
 */
int get_request(int fd_client, char* rqst) {
    char buf[MAX_BUFFER_RR_SIZE];
    ssize_t n_rb = read(fd_client, buf, MAX_BUFFER_RR_SIZE);
    if(n_rb <=0) {
        return -1;
    }

    return 0;
}

/**
 * Writing the response for the client.
 * @param fd_sckt
 * @param msg
 * @return
 */
int send_msg(int fd_sckt, char** msg) {

    for(int i=0; msg[i] != NULL; i++) {
        if(write(fd_sckt, msg[i], strlen(msg[i])) < 0) {
            return -1;
        }
    }

    return 0;
}

/**
 *
 * @param fd_clnt_sckt file descriptor for client connection socket
 * @return 0 if the request is processed successfully, -1 otherwise.
 */
int process_request(int fd_clnt_sckt) {

    char* rqst = calloc(sizeof(char), MAX_BUFFER_RR_SIZE);
    if(get_request(fd_clnt_sckt, rqst) < 0) {
        perror("error reading request");
    }

    printf("Request : %s\n", rqst);
    char** response = NULL;
    if(strcmp(rqst, CMD_LIST_DIR_SRTD_NAME) == 0) {
       handle_listdir_rqst(CMD_LIST_DIR_SRTD_NAME, response);
    }

    free(rqst);
    free_array((void **) response);
    return 0;
}
