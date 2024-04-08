#ifndef ERROR_CODES
#define EXIT_ERROR_CODE 99
#define EXIT_SUCCESS_CODE 0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>

//user header includes
#include "srvrutil.h"
#include "fileutil.h"
#include "srvr_attributes.h"


void start_server(const struct sockaddr_in address_srvr, int fd_srvr) {

    int fd_sckt = 0, no_of_clients = 0;
    socklen_t sckt_address_len = sizeof(address_srvr);

    // waiting for client
    printf("Waiting for client...\n");
    // wait in infinite loop to accept the client input
    while (1)
    {
        //TODO:
//        if(errno == ) check if the connection has been reset by the peer.
        if ((fd_sckt = accept(fd_srvr, (struct sockaddr *)&address_srvr, (socklen_t *)&sckt_address_len)) < 0)
        {
            perror("Error in accept");
//            sendfile();
            exit(EXIT_FAILURE);
        }

        // load balancing from server to mirror
        // if active clients less than =6 or is an odd no. after 12 connections
        // to be handled by server
        if (no_of_clients <= 6 || (no_of_clients > 12 && no_of_clients % 2 == 1))
        {
            /// handle by server
            // sedn control message to client "CTS(Connected to server)"
            send_msg(fd_sckt, "CTS");

            printf("New connection from client: %s...\n", inet_ntoa(default_server_address.sin_addr));

            /// fork a child and call process client func
            pid_t pid = fork();
            if (pid == 0)
            {
                // child process
                close(fd_srvr);

                // call process client function
                process_request(fd_sckt);
                exit(EXIT_SUCCESS);
            }
            else if (pid == -1)
            {
                // else failed to fork
                // error
                perror("Error processing client request");
                exit(EXIT_SUCCESS_CODE);
            }
            else
            {
                // parent process
                close(fd_sckt);

                while (waitpid(-1, NULL, WNOHANG) > 0); // clean up zombie processes
            }
        }
        else
        {
            //TODO:
            // redirecting to mirror server
            // printf("Redirecting to mirror\n");
//            redirect_to_mirror(new_skt);
        }

        // increase counter for no of connections
        no_of_clients = (no_of_clients + 1) % INT_MAX;
    }
}

int main(int argc, char* argv[]) {

    int fd_server = -1, _ret_val;

    _ret_val = init_server(&fd_server, default_server_address);
    if(_ret_val < 0 || fd_server < 0) {
        perror("server init setup failed : ");
        exit(EXIT_ERROR_CODE);
    }

    start_server(default_server_address, fd_server);


    printf("Hello World !!");
    return 0;
}
