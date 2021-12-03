#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>


#define BUF_LEN 512
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5

unsigned short int opCode;
unsigned short int * opCode_ptr;






void Sig_Child(int sign) {
    pid_t pid;
    int stat;

    while( (pid = waitpid(-1,&stat,WNOHANG)) > 0) {
        printf("Parent watches Child %d terminated with status %d.\n",pid,stat);
    }
}


void handler(char* filename, struct sockaddr_in *client_sock, int TID)
{
    if(opCode == RRQ)
    {
        int sock;
        FILE *fp;
        int blockNum = 1;
        // create socket
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
            perror("Socket");
            exit(1);
        }

        struct sockaddr_in sock_addr;
        socklen_t len = sizeof(sock_addr);
        memset(&sock_addr, 0, len);

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_addr.s_addr = INADDR_ANY;
        sock_addr.sin_port = htons(TID);

        if (bind(sock, (const struct sockaddr *) &sock_addr,len) < 0){
            perror("Binding");
            exit(1);
        }
        getsockname(sock, (struct sockaddr *) &sock_addr, &len);
        fflush(stdout);

        fp = fopen(filename+2,"rb");

        if(fp == NULL)
        {
            exit(1);
        }


        int read_len = BUF_LEN;
        // This loop will run as long as there 512 bytes to be read
        while(read_len == BUF_LEN)
        {

            char read[516];
            read[0] = '\0';
            read[1] = '\3';
            read[2] = (char)(blockNum>>8);
            read[3] = (char)blockNum;

            // Store data in a buffer
            read_len = fread(read+4, 1, BUF_LEN, fp);
            int n = sendto(sock, read, read_len+4, 0, (const struct sockaddr *) &client_sock, sizeof (struct sockaddr_in));

            char ack[4];
            socklen_t len = sizeof(client_sock);
            recvfrom(sock,ack, 4, MSG_WAITALL,(struct sockaddr *) &client_sock, &len);
            // Check if we will need to transfer more packets
            blockNum++;
        }
        close(sock);
        exit(1);
    }

    //Write request handler
    else if(opCode == WRQ){
        int sock;
        FILE* fp;
        ssize_t n;
        int block = 0;
        int blocknumber;

        char ack[4];
        //char ack2[4];
        int current_blocknumber = 1;
        //initialize a socket for requests(client)
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
            perror("Socket");
            exit(1);
        }

        struct sockaddr_in sock_addr;
        socklen_t len = sizeof(sock_addr);
        memset(&sock_addr, 0, len);

        sock_addr.sin_family = AF_INET;
        sock_addr.sin_addr.s_addr = INADDR_ANY;
        sock_addr.sin_port = htons(TID);

        if (bind(sock, (const struct sockaddr *) &sock_addr,len) < 0){
            perror("Binding");
            exit(1);
        }
        getsockname(sock, (struct sockaddr *) &sock_addr, &len);
        fflush(stdout);

        fp = fopen(filename+2, "wb");

        //get filename from the buffer and open
        if(fp == NULL)
        {
            exit(1);
        }

        //send ACK back
        opCode_ptr = (unsigned short int *)ack;
        opCode = ntohs(*opCode_ptr);
        *opCode_ptr = htons(ACK);
        *(opCode_ptr + 1) = htons(block);

        n = sendto(sock,ack,4,0,(const struct sockaddr *) &client_sock,sizeof(client_sock));

        if(n<0)
        {
            perror("ACK Error");
        }
        char write1[2048];
        do{
            //printf("do\n");
            //write into the file
            socklen_t len = sizeof(client_sock);
            n = recvfrom(sock, write1,2048,MSG_WAITALL,(struct sockaddr *) &client_sock,&len);
            fflush(stdout);
            opCode_ptr = (unsigned short int *)write1;
            opCode = ntohs(*opCode_ptr);
            if (opCode != DATA){
                exit(1);
            }
            blocknumber = ntohs(*(opCode_ptr + 1));
            char ack[4];
            ack[0] = '\0';
            ack[1] = '\4';
            ack[2] = (char)blocknumber>>8;
            ack[3] = (char)blocknumber;

           // printf("block number: %d\n",blocknumber);


            // ack[2] = ((unsigned int)buffer[2]);
            // ack[3] = ((unsigned int)buffer[3]);

            if(blocknumber == current_blocknumber){
                fwrite( write1+4,sizeof(char), n-4,fp);
                current_blocknumber++;
            }

            int Send;
            Send = sendto(sock,ack,4,0,(const struct sockaddr *) &client_sock,sizeof(client_sock));
            //printf("doend\n");
        }
        while(n == 516);
        fclose(fp);
        alarm(0);
        close(sock);
        exit(0);
    }
}

int main(int argc, char *argv[]){


    int port_min = atoi(argv[1]);
    int port_max = atoi(argv[2]);
    int TID = port_max;

    struct sockaddr_in client;
    ssize_t n;
    char buffer[516];
    int server_socket;

    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    server.sin_family = PF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_min);

    if((server_socket = socket(PF_INET, SOCK_DGRAM, 0))<0){
        perror("socket");
        exit(-1);
    }

    if(bind(server_socket, (struct sockaddr *)&server, server_len)<0){
        perror("bind");
        exit(-1);
    }

    getsockname(server_socket, (struct sockaddr *)&server, &server_len);
    fflush(stdout);
    signal(SIGCHLD, Sig_Child);

    while(1){
        pid_t pid;

        socklen_t client_len = sizeof(client);

        n = recvfrom(server_socket, buffer, 1024, MSG_WAITALL, (struct sockaddr *) &client, &client_len);
        if(n < 0) {
            perror("recvfrom");
            exit(-1);
        }


        opCode_ptr = (unsigned short int *)buffer;
        opCode = ntohs(*opCode_ptr);


        if(opCode != RRQ && opCode != WRQ)
        {
            char error[5];
            opCode_ptr = (unsigned short int *)error;
            *opCode_ptr = htons(ERROR);
            *(opCode_ptr + 1) = htons(4);
            *(error+ 4) = 0;
            sendto(server_socket,error,5,0,(const struct sockaddr*) &client,sizeof(client));
            exit(1);
        }
        else{
            if((pid = fork()) == 0){
                if(buffer[1] == RRQ || buffer[1] == WRQ)
                {
                    handler(buffer, &client, TID);
                    exit(0);
                }

            }
        }
        if (TID != port_min){
            TID --;
        }
    }
    return 0;
}
