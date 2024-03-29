//
// Created by Anuj Puri on 2024-03-25.
//

#ifndef FILE_DOWNLOAD_SERVER_SRVRUTIL_H
#define FILE_DOWNLOAD_SERVER_SRVRUTIL_H

#define MAX_BUFFER_SIZE 1024 // max buffer size for reading or writing the data from the Live socket.
#define MAX_QUEUE_SIZE 100 //defining the number of request queued, before rejection triggers.
#define _XOPEN_SOURCE 500
#define PORT 8080
#define MIRROR_PORT 7001

char request[MAX_BUFFER_SIZE];
char response[MAX_BUFFER_SIZE];

#endif //FILE_DOWNLOAD_SERVER_SRVRUTIL_H



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>

//integer return
int is_linux();
int process_request();
int send_msg();
int bind_address();
int listen_sckt();

/**
 *
 * @param fd_server
 * @param address_srvr
 * @param port_local
 * @return
 */
int init_server(int fd_server, struct sockaddr_in* address_srvr, int port_local);


int init_server(int fd_server, struct sockaddr_in* address_srvr, const int port_local) {
    int serv_fd, new_skt;
    struct sockaddr_in serv_addr, cli_addr;
    int opt = 1; //option for server socket
    int addrlen = sizeof(serv_addr); //address length
    int no_of_clients = 1; // keep count of no. of clients

    // Create socket file descriptor
    // use default protocol (i.e TCP)
    if ((fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // error handle
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /// Attributes for binding socket with IP and PORT
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // accepts any address
    serv_addr.sin_port = htons(port_local);

    // Bind socket to the PORT
    if (bind(fd_server, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {	//error
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen_sckt to the socket
    // queue of size BACKLOG
    if (listen(fd_server, MAX_QUEUE_SIZE) < 0)
    {
        perror("Error while listening..");
        exit(EXIT_FAILURE);
    }

    return 0;
}