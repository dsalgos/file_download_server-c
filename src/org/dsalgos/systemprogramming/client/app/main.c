#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXSIZE 1024

int main(int argc, char *argv[])
{
    struct sockaddr_in server_info;
    struct hostent *he;
    int socket_fd,num;
    char *buffer = malloc(sizeof(char) * 1024);
    char *buffer_copy = malloc(sizeof(char) * 1024);

    char buff[1024];

    if (argc != 2) {
        fprintf(stderr, "Usage: client hostname\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1]))==NULL) {
        fprintf(stderr, "Cannot get host name\n");
        exit(1);
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
        fprintf(stderr, "Socket Failure!!\n");
        exit(1);
    }

    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(PORT);
    server_info.sin_addr = *((struct in_addr *)he->h_addr);
    if (connect(socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr))<0) {
        perror("connect");
        exit(1);
    }

    printf("connected with fd %d\n", socket_fd);

    while (1) {
        printf("client$ ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, 1024, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';

        // printf("%s ", buffer);

        send(socket_fd,buffer, strlen(buffer),0);

        memset(buffer, 0, sizeof(buffer));


        while((num =  read(socket_fd, buffer, 1024)) > 0) {
            printf("inside while\n");
            printf("%s", buffer);
            if(num < 1024) {
                break;
            }
        }
        printf("Received Response:\n%s\n", buffer);
    }
    close(socket_fd);

}
