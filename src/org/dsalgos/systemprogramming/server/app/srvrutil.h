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

char request[MAX_BUFFER_RR_SIZE];
char response[MAX_BUFFER_RR_SIZE];

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
int process_request(int fd_sckt);
int send_msg(int fd_sckt, char* msg);
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


int send_msg(int fd_sckt, char* msg) {
    printf("Sending message...");
    return 0;
}

int process_request(int fd_sckt) {
    printf("processing the request...");
    return 0;
}
