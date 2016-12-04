/* This program sets up a socket to allow two clients to talk to each other */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <netdb.h>


pthread_mutex_t mulo = PTHREAD_MUTEX_INITIALIZER;

int total;
int portNumber;                 /* Port Number   */
int ClientSocket;
int ServerSocket;               /* Listen socket */
socklen_t ClientLength;         /* len of sockaddr */
struct sockaddr_in ServerStructure, ClientStructure;

void *socket_connection(void *passedvalue);

int main(int argc, char *argv[]) {
    pthread_mutex_init(&mulo, NULL);

    if (argc < 2) { fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* Create the socket. */
    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { perror("socket"); exit(1); }

    /* Create the address of the server.*/
    portNumber = atoi(argv[1]);
    bzero((char *) &ServerStructure, sizeof(ServerStructure));
    ServerStructure.sin_family = AF_INET;
    ServerStructure.sin_addr.s_addr = INADDR_ANY;
    ServerStructure.sin_port = htons(portNumber);


    /*Bind the socket to the address.*/
    if (bind(ServerSocket, (struct sockaddr *) &ServerStructure, sizeof(ServerStructure)) < 0) { perror("bind"); exit(1); }

    /* Listen for connections. */
    if (listen(ServerSocket, 5) < 0) { perror("listen"); exit(1); }


    pthread_t array[3];	  // this is valid to set multiple variables
    ClientLength = sizeof(ClientStructure);
    int i = 0;
    /*Accept a connection.*/
    for (;;) {
        ClientSocket = 0;
        if ((ClientSocket = accept(ServerSocket, (struct sockaddr *) &ClientStructure, &ClientLength)) < 0) {
            perror("accept");
            exit(1);
        }
        printf("Client Connected Successfully \n");
        pthread_create(&array[i], NULL, socket_connection, (void *) &total);
    }
}

void * socket_connection(void *passedvalue) {
    char buffer[1024];
    ssize_t bytesRead;
    fd_set DataArray;     /* Set of file descriptors to poll */

    int passed = 1;

    while(passed != 0) {
        int *passed_in_value = ((int *) passedvalue);

        /* Set up polling using select. */
        FD_ZERO(&DataArray);
        FD_SET(ClientSocket, &DataArray);

        /* Wait for some input. */
        select((ClientSocket + 1), &DataArray, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        /* If either descriptor has some input, read it and copy it to the other. */

        if (FD_ISSET(ClientSocket, &DataArray)) {
            bytesRead = recv(ClientSocket, buffer, sizeof(buffer), 0); // flags = 0
            /* If error or eof, terminate. */
            if (bytesRead < 1) {
                close(ClientSocket);
                exit(0);
            }

            pthread_mutex_lock(&mulo);
            passed = atoi(buffer);
            *passed_in_value += passed;

            pthread_mutex_unlock(&mulo);

            buffer[0] = '\0';

            sprintf(buffer, "%d", *passed_in_value);

            write(1, buffer, (size_t) bytesRead);

            send(ClientSocket, buffer, (size_t) bytesRead, 0);
        }
    }
}