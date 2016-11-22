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
#include <netdb.h>

#define TotalT 25

struct {
    int unique;
    int taken;
} ticket[TotalT];

int main(int argc, char *argv[]) {
        char buffer[1024];		        /* Buffer for messages to others. */
        fd_set FileArray;               /* Set of file descriptors to poll */

        struct sockaddr_in ServerStructure, Client1Structure, Client2Structure;

        int ServerSocket;               /* Listen socket */
        int portNumber;                 /* Port Number   */
        socklen_t ClientLength;         /* len of sockaddr */
        int Client1Socket;              /* Socket for first connection. */
        int Client2Socket;              /* Socket for second connection. */
        int EOFileArray;                /* descriptors up to maxfd-1 polled */
        int SocketsReady;               /* # descriptors ready. */
        int bytesRead;                  /* # chars on read() */

        fd_set DataArray;                     /* Set of file descriptors to poll */

        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }

        printf("\n[SERVER]: Database Table: \n__________________________ \n");
        for (int i = 0; i != 25; i++) {
            ticket[i].unique = rand() % 99999;
            ticket[i].taken = 0;
            printf("[Tkt#  %i] %i : %s \n", i, ticket[i].unique, (ticket[i].taken == 0) ? "AVAIL" : "SOLD");

        } printf("__________________________ \n Waiting for Connections \n");


        /* Create the socket. */
        if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
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
        	perror( "listen");
        	exit(1);
        }


        ClientLength = sizeof(Client1Structure);
        /*Accept a connection.*/
        if ((Client1Socket = accept(ServerSocket, (struct sockaddr *) &Client1Structure, &ClientLength)) < 0) {
        	perror("accept");
        	exit(1);
        } else { printf("Client 1 Connected Successfully \n"); }

        /* Accept another . */
        if ((Client2Socket = accept(ServerSocket, (struct sockaddr *) &Client2Structure, &ClientLength)) < 0) {
        	perror("accept");
        	exit(1);
        } else { printf("Client 2 Connected Successfully \n"); }

        EOFileArray = (Client1Socket > Client2Socket ? Client1Socket : Client2Socket) + 1;
        while (1) {
                /* Set up polling using select. */
                FD_ZERO(&DataArray);
                FD_SET(Client1Socket,&DataArray);
                FD_SET(Client2Socket,&DataArray);

                /* Wait for some input. */
                SocketsReady = select(EOFileArray, &DataArray, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

                /* If either descriptor has some input, read it and copy it to the other. */

                if( FD_ISSET(Client1Socket, &DataArray)) {
                    bytesRead = recv(Client1Socket, buffer, sizeof(buffer), 0); // flags = 0
                        /* If error or eof, terminate. */
                        if(bytesRead < 1) {
                                close(Client1Socket);
                                close(Client2Socket);
                                exit(0);
                        }

                        if (buffer == )
                        printf("")

                        send(Client2Socket, buffer, bytesRead, 0);
                }

                if( FD_ISSET(Client2Socket, &DataArray)) {
                    bytesRead = recv(Client2Socket, buffer, sizeof(buffer), 0);
                        /* If error or eof, terminate. */
                        if(bytesRead < 1) {
                                close(Client1Socket);
                                close(Client2Socket);
                                exit(0);
                        }
                        send( Client1Socket, buffer, bytesRead, 0);
                }
        } 
}

