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

    char data[1024];                /* data for messages to others. */
    int ServerSocket;          /* This end of connection*/
    int portNumber;

    struct sockaddr_in ServerStructure;
    struct hostent *server;

    int EOFileArray;                /* descriptors up to maxfd-1 polled */
    int SocketsReady;               /* # descriptors ready. */
    ssize_t bytesRead;              /* # chars on read() . */

    fd_set FileArray;     /* Set of file descriptors to poll */


    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) { perror("socket"); exit(1); }

    /* Create the address of the server. */
    portNumber = atoi(argv[2]);
    
    if ((server = gethostbyname(argv[1])) == NULL) { fprintf(stderr, "ERROR, no such host\n"); exit(0); }
    
    bzero((char *) &ServerStructure, sizeof(ServerStructure));
    ServerStructure.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &ServerStructure.sin_addr.s_addr, server->h_length);
    ServerStructure.sin_port = htons(portNumber);

    /*Connect to the server.*/
    if (connect(ServerSocket, (struct sockaddr *) &ServerStructure, sizeof(ServerStructure)) < 0) { perror("connect"); exit(1); }

    int total = 0;
    while (1) {

        /* Wait for some input. */
        printf("Enter CLIENT 2 Data: ");
        scanf("%s", data);
        bytesRead = sizeof(data);
        send(ServerSocket, data, (size_t) bytesRead, 0);
        if (bytesRead < 1) { close(ServerSocket); exit(0); }

        
        /* Wait for server */
        bytesRead = read(ServerSocket, data, sizeof(data));
        if (bytesRead < 1) { close(ServerSocket); exit(0); }
        total = atoi(data);
        printf("SERVER Total: %i\n", total);
        // write(1, data, bytesRead);
    
    }
}


