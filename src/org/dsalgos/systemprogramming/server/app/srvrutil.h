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
char* MSG_RES_SERVER = "RESPONSE :\n";
char* MSG_RES_SERVER_404 = "File not found.\n";

void handle_listdir_rqst(int fd, const char* command, char*** response);
void handle_fs_name(int fd, char** rqst);
void handle_fs_size(int fd, char** rqst);
void handle_fs_date(int fd, char** rqst);


/**
 *
 * @param fd_server
 * @param address_srvr
 * @param port_local
 * @return
 */
int init_server(int *fd_server, struct sockaddr_in address_srvr);

//integer return
int process_request(int fd_clnt_sckt);
int send_msg_chars(int fd_sckt, char** msg);

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

    /// Attributes for binding socket with IP and PORT
    memset(&address_srvr, 0, sizeof(address_srvr));
    address_srvr.sin_family = AF_INET;
    address_srvr.sin_addr.s_addr = INADDR_ANY; // accepts any address
    address_srvr.sin_port = htons(PORT);

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



void handle_listdir_rqst(int fd, const char* command, char*** response) {

    if(command == NULL) { return; }

    *response = NULL;
    char* dir_home = expand_if_tilda("~");
    printf("searching find in home %s\n", dir_home);

    struct dentry* list_dentry = NULL;
    int count_dir = -1;
    if(strcmp(CMD_LIST_DIR_SRTD_NAME, command) == 0) {
        count_dir = list_dir_sort(dir_home, &list_dentry, d_compare_name);
    } else if(strcmp(CMD_LIST_DIR_SRTD_MTIME, command) == 0){
        count_dir = list_dir_sort(dir_home, &list_dentry, d_compare_modified);
    }

    printf(" handle_listdir_rqst : found number of directories : %d\n", count_dir);
    if(list_dentry != NULL) {

        printf(" list is not NULL \n");
        *response = (char**)malloc(sizeof(char*) * count_dir);
        if (*response == NULL) {
            perror("malloc");
            return;
        }
        for(int i=0; i < count_dir; i++) {
            size_t length = strlen(list_dentry[i].f_name)+2;

            //create the response to be sent
            (*response)[i] = (char*) malloc(sizeof(char) * length);
//            //add the file name to response
            (*response)[i]  = strdup(list_dentry[i].f_name);
            (*response)[i][strlen(list_dentry[i].f_name)] = '\n';
            (*response)[i][length-1] = '\0';

        }

        for(int i = 0; i < count_dir; i++) {
            printf("copied directory is %s", (*response)[i]);
        }

        send_msg_chars(fd, *response);
    }


    free(dir_home);
    free_dentry(list_dentry, count_dir);
}

void handle_fs_name(int fd_clnt_sckt, char** rqst) {
    char* dir_home = expand_if_tilda("~");

    char* file_name = calloc(sizeof(char), strlen(rqst[1]));
    strcpy(file_name, rqst[1]);

    //calling the function to check if the file exists
    //and get the required associated details.
    struct fdetails* fs_details = file_search(dir_home, file_name);

    if(fs_details != NULL) {
        ssize_t error = 0;
        error = write(fd_clnt_sckt, MSG_RES_SERVER, strlen(MSG_RES_SERVER)) < 0 ? -1 : error;
        error = write(fd_clnt_sckt, fs_details->f_name, sizeof(fs_details->f_name)) < 0 ? -1 : error;
        error = write(fd_clnt_sckt, C_NEW_LINE, sizeof(char)) < 0 ? -1 : error;
        error = write(fd_clnt_sckt, fs_details->f_name, sizeof(fs_details->f_name)) < 0 ? -1 : error;
        error = write(fd_clnt_sckt, C_NEW_LINE, sizeof(char)) < 0 ? -1 : error;

        if (error != 0) {
            perror("Error while sending file details using name ");
        }
    } else {
        if(write(fd_clnt_sckt, MSG_RES_SERVER_404, strlen(MSG_RES_SERVER_404)) < 0) {
            perror("Error : ");
        }
    }
}

/*
 * reading request from the client
 */
int get_request(int fd_client, char* rqst, size_t sz_rqst) {


    ssize_t n_rb = read(fd_client, rqst, sz_rqst);
    if(n_rb <=0) {
        return -1;
    }

    return 0;
}

void handle_fs_size(int fd, char** rqst) {
//    lsw
}


/**
 * Writing the response for the client.
 * @param fd_sckt
 * @param msg
 * @return
 */
int send_msg_chars(int fd_sckt, char** msg) {
    for(int i=0; msg[i] != NULL; i++) {

        if(write(fd_sckt, msg[i], strlen(msg[i])) < 0) {
            perror("write failed ");
            return -1;
        }
    }

    return 0;
}

/**
 * Processing the client request. This untility function is expected
 * to be called
 * @param fd_clnt_sckt file descriptor for client connection socket
 * @return 0 if the request is processed successfully, -1 otherwise.
 */
int process_request(int fd_clnt_sckt) {

    char* rqst = malloc(sizeof(char)* MAX_BUFFER_RR_SIZE);
    if(get_request(fd_clnt_sckt, rqst, MAX_BUFFER_RR_SIZE) < 0) {
        perror("error reading request");
    }

    int num_tokens;
    char** cmd_vector = tokenize(rqst, C_SPACE, &num_tokens);
    if(cmd_vector == NULL || num_tokens < 2) {
        perror("error tokenizing request");
        return -1;
    }

    printf("Request : %s\n", rqst);
    char **response = NULL;
    if(strcmp(rqst, CMD_LIST_DIR_SRTD_NAME) == 0) {
       handle_listdir_rqst(fd_clnt_sckt, CMD_LIST_DIR_SRTD_NAME, &response);
    } else if(strcmp(rqst, CMD_LIST_DIR_SRTD_MTIME) == 0) {
        handle_listdir_rqst(fd_clnt_sckt, CMD_LIST_DIR_SRTD_MTIME, &response);
    } else if(strcmp(cmd_vector[0], CMD_FILE_SRCH_NAME) == 0) {
        handle_fs_name(fd_clnt_sckt, cmd_vector);
    } else if(strcmp(cmd_vector[0], CMD_FILE_SRCH_SIZE) == 0) {
        handle_fs_size(fd_clnt_sckt, cmd_vector);
    } else if(strcmp(cmd_vector[0], CMD_FILE_SRCH_DATE) == 0) {
//        handle_fs_date(fd_clnt_sckt, cmd_vector);
    } else if(strcmp(cmd_vector[0], CMD_CLIENT_QUIT) == 0) {
        //Quit must release all resources exclusively
        free(rqst);
        free_array((void **) response);
        free_array((void **) cmd_vector);
        close(fd_clnt_sckt);
        exit(EXIT_SUCCESS);
    }

    //free up the memory
    free(rqst);
    free_array((void **) response);
    free_array((void **) cmd_vector);
    printf("done with processing the request ...\n");
    return 0;
}
