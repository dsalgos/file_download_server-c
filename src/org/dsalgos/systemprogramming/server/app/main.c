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

//user header includes
#include "srvrutil.h"

//constants
#define BUFSIZE 2048
#define _XOPEN_SOURCE 500
#define PORT 8080
#define MIRROR_PORT 7001
#define BACKLOG 200

char request[BUFSIZE];
char response[BUFSIZE];


int init_server(int fd_server, struct sockaddr_in* address_srvr) {
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
    serv_addr.sin_port = htons(PORT);

    // Bind socket to the PORT
    if (bind(fd_server, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {	//error
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen_sckt to the socket
    // queue of size BACKLOG
    if (listen(fd_server, BACKLOG) < 0)
    {
        perror("Error while listening..");
        exit(EXIT_FAILURE);
    }


    return 0;
}

int main(int argc, char* argv[]) {

    printf("Hello , welcome to the server ...\n");

    return 0;
}
