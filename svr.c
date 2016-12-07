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

#include <errno.h>
#include <netdb.h>


pthread_mutex_t mulo = PTHREAD_MUTEX_INITIALIZER;

int total;
int portNumber;
/* Port Number   */
int ServerSocket;
/* Listen socket */
struct sockaddr_in ServerStructure, ClientStructure;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void *socket_connection(void *passedvalue);

int main(int argc, char *argv[]) {
    pthread_mutex_init(&mulo, NULL);


    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    /* Create the socket. */
    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Create the address of the server.*/
    portNumber = atoi(argv[1]);
    bzero((char *) &ServerStructure, sizeof(ServerStructure));
    ServerStructure.sin_family = AF_INET;
    ServerStructure.sin_addr.s_addr = INADDR_ANY;
    ServerStructure.sin_port = htons(portNumber);


    /*Bind the socket to the address.*/
    if (bind(ServerSocket, (struct sockaddr *) &ServerStructure, sizeof(ServerStructure)) < 0) {
        perror("bind");
        exit(1);
    }

    /* Listen for connections. */
    if (listen(ServerSocket, 5) < 0) {
        perror("listen");
        exit(1);
    }


    int i = 0;
    pthread_t array[6];
    /*Accept a connection.*/
    for (; ;) {
        pthread_create(&array[i], NULL, socket_connection, (void *) &total);
        pthread_cond_wait(&c, &mulo);
    }
}

void *socket_connection(void *passedvalue) {

    // this is valid to set multiple variables
    struct sockaddr_in ClientStructure;
    int ClientLength = sizeof(ClientStructure);
    int ClientSocket;

    if ((ClientSocket = accept(ServerSocket, (struct sockaddr *) &ClientStructure, &ClientLength)) < 0) {
        perror("accept");
        exit(1);
    }
    printf("Client Connected Successfully \n");


    char buffer[1024] = "/0";
    ssize_t bytesRead;

    int passed = 1;

    printf("enter the while \n");

    while (passed != 0) {
        int *passed_in_value = ((int *) passedvalue);
        /* If either descriptor has some input, read it and copy it to the other. */

        printf("fd entered \n");

        bytesRead = recv(ClientSocket, (void *) buffer, sizeof(buffer), 0); // flags = 0
        printf("Buffer read\n");
        /* If error or eof, terminate. */
        //sprintf(buffer, "%d", *passed_in_value);



        pthread_mutex_lock(&mulo);
        passed = atoi(buffer);
        *passed_in_value += passed;
        pthread_mutex_unlock(&mulo);

        if (bytesRead < 1) {
            printf("bytes read fail: %d\n ---> %s\n ---> %d\n ---> %d\n ---> %i\n", bytesRead, buffer,
                   passed,
                   *passed_in_value, errno);

            close(ClientSocket);
            exit(0);
        }

        pthread_cond_signal(&c);

        buffer[0] = '\0';

        sprintf(buffer, "%d", *passed_in_value);

        printf("passed in value: %i\n", *passed_in_value);

        //write(1, buffer, (size_t) bytesRead);

        send(ClientSocket, buffer, (size_t) bytesRead, 0);


        printf("\nend of the while\n");
    }
}