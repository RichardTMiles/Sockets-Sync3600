#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/socket.h> 
#include <sys/un.h>
#include <fcntl.h>

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {

    char buffer[1024];                /* Buffer for messages to others. */
    int ServerSocket;          /* This end of connection*/
    int portNumber;

    struct sockaddr_in ServerStructure;
    struct hostent *server;

    int EOFileArray;                /* descriptors up to maxfd-1 polled */
    int SocketsReady;               /* # descriptors ready. */
    int bytesRead;                  /* # chars on read() . */

    fd_set FileArray;     /* Set of file descriptors to poll */


    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Create the address of the server. */
    portNumber = atoi(argv[2]);
    if ((server = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &ServerStructure, sizeof(ServerStructure));
    ServerStructure.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &ServerStructure.sin_addr.s_addr, server->h_length);
    ServerStructure.sin_port = htons(portNumber);

    /*Connect to the server.*/
    if (connect(ServerSocket, (struct sockaddr *) &ServerStructure, sizeof(ServerStructure)) < 0) {
        perror("connect");
        exit(1);
    }


    EOFileArray = ServerSocket + 1;
    while (1) {
        /* Set up polling. */
        FD_ZERO(&FileArray);
        FD_SET(ServerSocket, &FileArray);
        FD_SET(0, &FileArray);

        /* Wait for some input. */
        SocketsReady = select(EOFileArray, &FileArray, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        /* If either device has some input,
           read it and copy it to the other. */

        if (FD_ISSET(ServerSocket, &FileArray)) {
            bytesRead = recv(ServerSocket, buffer, sizeof(buffer), 0);

            /* If error or eof, terminate. */
            if (bytesRead < 1) {
                close(ServerSocket);
                exit(0);
            }
            // write to stdout = 1 ; the buffer; # bites
            write(1, buffer, bytesRead);
        }

        if (FD_ISSET(0, &FileArray)) {
            bytesRead = read(0, buffer, sizeof(buffer));

            /* If error or eof, terminate. */
            if (bytesRead < 1) {
                close(ServerSocket);
                exit(0);
            }
            send(ServerSocket, buffer, bytesRead, 0);
        }
    }

}

