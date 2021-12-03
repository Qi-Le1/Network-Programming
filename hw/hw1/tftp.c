#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#define ACK 4
#define ERROR 5

unsigned short int opcode;
unsigned short int * opcode_ptr;


//helper functions for handler and main.
void sigchild(int signo) {
    pid_t pid;
    int stat;

    while( (pid = waitpid(-1,&stat,WNOHANG)) >0) {
        printf("Parent watches Child %d terminated with status %d.\n",pid,stat);
    }
}
//this helper function to send error msg.
void Error(int sock,int ecode, struct sockaddr_in addr){
    char err[5];
    opcode_ptr = (unsigned short int *)err;
    *(opcode_ptr + 1) = htons(ecode);
    *(err+ 4) = 0;
    *opcode_ptr = htons(ERROR);
    sendto(sock,err,5,0,(const struct sockaddr*) &addr,sizeof(addr));

}
//this used to send msg
void sending( int sock, char* buffer, int len, struct sockaddr_in addr){
    int n = sendto(sock, buffer, len, 0, (const struct sockaddr *) &addr,sizeof (struct sockaddr_in));
    if(n==-1)
    {
      perror("Send Error");
    }
}
int getBlock(char * buffer){
    unsigned short int * opcode_ptr;
    opcode_ptr = (unsigned short int *)buffer;
    return  ntohs(*(opcode_ptr + 1));
}
int open_addr(int port){
    int sock;

    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Socket Wrong! ");
        return -1;
    }

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    memset(&client_addr, 0, len);

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(port);
    //error check.
    if (bind(sock, (const struct sockaddr *) &client_addr,len) < 0){
        perror("?????");
        return -1;
    }

    getsockname(sock, (struct sockaddr *) &client_addr, &len);
    fflush(stdout);
    return sock;
}

//read handler and writer handler.
void Read_handle(char* filename, struct sockaddr_in addr, int TID){

    int sock;
    FILE* fd;
    int block=1;
    int readlen = 512;
    sock = open_addr(TID);

    fd=fopen(filename+2,"rb");
    if(fd==NULL)
    {
        printf("Error!!!");
        Error(sock,1,addr);
        exit(1);
    }
    while(readlen == 512)
    {
        char buffer1[516];
        buffer1[0] = '\0';
        buffer1[1] = '\3';
        buffer1[2] = (char)(block>>8);
        buffer1[3] = (char)block;

        readlen = fread(buffer1+4, sizeof(char), 512, fd);

        sending(sock,buffer1,readlen+4, addr);

        char ack[4];
        socklen_t len = sizeof(addr);
        recvfrom(sock,ack,4,MSG_WAITALL,(struct sockaddr *) &addr,&len);

        block++;
    }
    close(sock);
    exit(0);

}
void Write_handle(char* filename, struct sockaddr_in addr,int TID){
    int sock;
    FILE* fd;
    ssize_t n;
    int blockNum;
    int block;

    char ack[4];
    sock =open_addr(TID);
    block = 0;
    int numbernow = 1;

    fd=fopen(filename+2,"wb");
    if(fd==NULL){
        Error(sock,1,addr);
        exit(1);
    }

    opcode_ptr = (unsigned short int *)ack;
    opcode = ntohs(*opcode_ptr);
    *opcode_ptr = htons(ACK);
    *(opcode_ptr + 1) = htons(block);

    n = sendto(sock,ack,4,0,(const struct sockaddr *) &addr,sizeof(addr));

    if(n<0)
    {
        perror("ACK Error");
    }
    char buffer[2048];
    do{
        socklen_t len = sizeof(addr);
        n = recvfrom(sock,buffer,2048,MSG_WAITALL,(struct sockaddr *) &addr,&len);
        fflush(stdout);
        opcode_ptr = (unsigned short int *)buffer;
        opcode = ntohs(*opcode_ptr);
        if (opcode != 3){
            Error(sock,4,addr);
            exit(1);
        }
        blockNum = getBlock(buffer);
        char ack[4];
        ack[0] = '\0';
        ack[1] = '\4';
        ack[2] = (char)blockNum>>8;
        ack[3] = (char)blockNum;

        if(blockNum == numbernow){
            fwrite( buffer+4,sizeof(char), n-4,fd);
            numbernow++;
        }

        int bytesSend;
        bytesSend = sendto(sock,ack,4,0,(const struct sockaddr *) &addr,sizeof(addr));
    }while(n==516);
    close(sock);
    fclose(fd);
    alarm(0);
    exit(0);
}

int main(int argc, char*argv[]){

    int port_lower = atoi(argv[1]);
    int port_upper = atoi(argv[2]);

    struct sockaddr_in client_client_addr;

    int TID = port_upper;
    char buffer[516];
    ssize_t n=0;
    int server;

    server = open_addr(port_lower);
    signal(SIGCHLD, sigchild);
    while(1)
    {
        pid_t pid;
        socklen_t addr_len = sizeof(client_client_addr);
        n = recvfrom(server, buffer, 2048, MSG_WAITALL,(struct sockaddr *)&client_client_addr, &addr_len);

        opcode_ptr = (unsigned short int *)buffer;
        opcode = ntohs(*opcode_ptr);

        if(opcode != 1 && opcode != 2) {
            Error(server, 4,client_client_addr);
            exit(1);
        }

        //2 handle
        if(buffer[1] == 1){
            pid=fork();
            if(pid==0){
                Read_handle(buffer,client_client_addr,TID);
                exit(0);
            }
        }
        if(buffer[1] == 2){
            pid=fork();
            if(pid==0){
                Write_handle(buffer,client_client_addr,TID);
                //change port
                exit(0);
            }
        }
        if (TID != port_lower){
            TID --;
        }

    }
    return 0;
}
