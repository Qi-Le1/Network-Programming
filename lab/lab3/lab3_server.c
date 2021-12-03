#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char** argv) {
    int port=9877+atoi(argv[1]);

    // creating a listening socket
    int listenfd;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( listenfd == -1 ) {
        perror( "socket() failed" );
        return EXIT_FAILURE;
    }

    struct sockaddr_in server;
    server.sin_family = PF_INET;  /* AF_INET */
    server.sin_addr.s_addr =  htonl(INADDR_ANY) ;
    server.sin_port = htons( port );
    int len = sizeof( server );

    if ( bind( listenfd, (struct sockaddr *)&server, len ) == -1 ) {
        perror( "bind() failed" );
        return EXIT_FAILURE;
    }

    if ( listen( listenfd, 5 ) == -1 ) {
        perror( "listen() failed" );
        return EXIT_FAILURE;
    }
    printf("listening on port %d\n", port);

    // accepting client connection
    struct sockaddr_in client;
    int fromlen = sizeof( client );

    int newsd = accept( listenfd, (struct sockaddr *)&client, (socklen_t *)&fromlen );
    printf( "Accepted connection\n");

    // get msg from stdin
    char buffer[BUF_SIZE];
    int n=0;

//    printf("input messages: \n");
    while(fgets(buffer, BUF_SIZE, stdin)) {

        // sending the input string to client
        n = send( newsd, buffer, sizeof(buffer), 0 );

        if ( n != sizeof(buffer) ) {
            perror( "send() failed" );
            return EXIT_FAILURE;
        }

        printf("succeed in sending msg: %s\n", buffer);

        memset((void*) buffer, 0, BUF_SIZE);

    }

    printf("Shutting down due to EOF\n");
    close(newsd);



}
