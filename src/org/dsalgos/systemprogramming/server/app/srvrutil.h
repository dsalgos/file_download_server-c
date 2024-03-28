//
// Created by Anuj Puri on 2024-03-25.
//

#ifndef FILE_DOWNLOAD_SERVER_SRVRUTIL_H
#define FILE_DOWNLOAD_SERVER_SRVRUTIL_H

#endif //FILE_DOWNLOAD_SERVER_SRVRUTIL_H

#define MAX_BUFFER_SIZE 256
#define MAX_QUEU_SIZE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>

//constant literals
const char C_NEW_LINE = '\n';
const char C_NULL = '\0';
const char C_SPACE = ' ';
const char C_TILDA = '~';



//string returns
char* trim(const char* str);
char* expand_if_tilda(char *command);

//integer return
int is_linux();
int is_txt_file(const char* f_name);
int copy_file(int fd_src, int fd_dest);
int open_file(const char* f_path, int oargs);
int process();
int send_msg();
int bind();
int listen_sckt();
int init_server(int fd_server, struct sockaddr_in* address_srvr, const int port_local);


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


    //creating a buffer, of size MAX_BUFFER_SIZE
    char* buffer = malloc(sizeof(char)* MAX_BUFFER_SIZE);

    //now, let's start the process of reading the data from the src file
    //read until there is nothing to be read or written.
    while(1) {
        long int rb = read(fd_src, buffer, MAX_BUFFER_SIZE);
        if(rb <=0) break;

        long int wb = write(fd_dest, buffer, MAX_BUFFER_SIZE);
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
    if (listen(fd_server, MAX_QUEU_SIZE) < 0)
    {
        perror("Error while listening..");
        exit(EXIT_FAILURE);
    }

    return 0;
}
