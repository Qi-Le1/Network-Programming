//
// Created by 果树猫 on 2019-09-26.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 1024

int main() {
    int conn=5;
    int fds[5], ports[5];
    int nready;
    int	maxfd=fileno(stdin);
    char buff[BUF_SIZE];
    fd_set rset, allset;
    int curfd;

    for(int i=0;i<5;i++){
        fds[i]=-1;
        ports[i]=0;
    }
    FD_ZERO(&allset);
    FD_SET(fileno(stdin), &allset);

    while(1){
        rset=allset;
        nready=select(maxfd+1, &rset, NULL, NULL, NULL);

        if(nready==0) continue;

        // new connection
        if(FD_ISSET(fileno(stdin), &rset) && conn>=0){

            fgets(buff, BUF_SIZE, stdin);
            int port=atoi(buff);
            if(port<9877) continue;
            printf("client stdin gets: %d\n", port);

            struct sockaddr_in server;
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);

            struct hostent * hp = gethostbyname( "127.0.0.1" );
            memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
            server.sin_family = AF_INET;
            server.sin_port = htons(port);

            connect(sockfd, (struct sockaddr *) &server, sizeof(server));
            FD_SET(sockfd, &allset);

            conn--;
            maxfd=maxfd>sockfd?maxfd:sockfd;

            for(int i=0;i<5;i++){
                if(fds[i]<0){
                    // save the fd
                    fds[i]=sockfd;
                    // save the port num
                    ports[i]=port;
                    break;
                }
            }

            printf("client connect to new server %d\n", sockfd);
            printf("now fd table is: \n");
            for(int i=0;i<5;i++) {
                printf("%d ", fds[i]);
                printf("%d\n", ports[i]);
            }
        }

        // check all servers for data
        for(int i=0;i<5;i++){
            if((curfd=fds[i])==-1) continue;
            if(FD_ISSET(curfd, &rset)){

                // server closed connection
                if ((read(curfd, buff, BUF_SIZE)) == 0) {
                    printf("server with port %d disconnected\n", ports[i]);
                    close(curfd);
                    FD_CLR(curfd, &allset);
                    fds[i]=-1;
                    ports[i]=0;
                    conn++;
                }
                else{
                    printf("%s\n", buff);
                }
            }

        }
    }

    return 0;
}
