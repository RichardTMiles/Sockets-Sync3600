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

//quick power function because I wanted one for ints
int power(int x, int y)
{
    int result = 1;
    int c = 0;
    for(; c < y; c++)
        result *= x;
    return result;
}




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

    FILE *fp;                       /*pointer to grab ouput of a ps command*/
    char path[200];                 //string to hold said output
    int cpusage = 0;                //int to hold cpu usage
    char command[100] = "ps -o\"%C\" -p       | sed -n '2p'";
    pid_t pid = getpid();           //id of this process
    int ipid = pid;                 //cast to an int
    int i = 4;                      //power of 10 ar which to start
    int conv;                       //converter to a single digit
    int c = 13;                     //counter starting at the spot in the command string where pid goes
    int maxuse                      //maximum allowable cpu usage

    //filling out pid of command string
    while(ipid > 0)
    {
        conv = ipid / power(10, i);
        command[c] = conv + 48;
        ipid -= conv * power(10, i);
        i--;
        c++;
    }

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

    //prompt user for maximum allowable cpu usage
    printf("What is the maximum cpu usage you would like to alot to this program?\n");
    scanf("%d", maxuse);
    //keep prompting until the number is acceptable
    while (maxuse < 1 || maxuse > 100) {
        printf("Please input a number between 1 and 100\n");
        scanf("%d", maxuse);
    }

    int total = 0;
    while (1) {

        //check current cpu usage
        fp = popen(command, "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        }
        else if (fgets(path, sizeof(path) - 1, fp) != NULL) {
            cpusage = atoi(&path[0]);
            printf("current usage: %d\n", cpusage);

            if(cpusage > maxuse) {
                /**********************************************
                  insert cpu overusage stuf here 
                ***********************************************/
            }
        }


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


