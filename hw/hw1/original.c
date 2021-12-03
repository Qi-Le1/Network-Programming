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

#define MAX_BUF 516
#define BUF_LEN 512
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5
unsigned short int opcode;
unsigned short int * opcode_ptr;
//unsigned short int *opcode_ptr_ack;
//child handler
void Sig_Child(int signo) {
    pid_t pid;
    int stat;

    while( (pid = waitpid(-1,&stat,WNOHANG)) >0) {
        printf("Parent watches Child %d terminated with status %d.\n",pid,stat);   
    }
}
//initialize connection helper function
int create_addr(int port){
    int sock;
    // create socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Socket");
        return -1;
    }

    struct sockaddr_in sock_addr;
    socklen_t len = sizeof(sock_addr);
    memset(&sock_addr, 0, len);

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(port);

    if (bind(sock, (const struct sockaddr *) &sock_addr,len) < 0){
        perror("Binding");
        return -1;
    }

    getsockname(sock, (struct sockaddr *) &sock_addr, &len);
    fflush(stdout);
    return sock;
}

//send to helper function 
void sender( int sock, char* buffer, int len, struct sockaddr_in addr){
    int n = sendto(sock, buffer, len, 0, (const struct sockaddr *) &addr,sizeof (struct sockaddr_in));
    if(n==-1){perror("Send Error");}     
}


//handle the error msg
void Error(int sock,int ercode, struct sockaddr_in addr){
    char error[5];
    opcode_ptr = (unsigned short int *)error;
    *opcode_ptr = htons(ERROR);
    *(opcode_ptr + 1) = htons(ercode);
    *(error+ 4) = 0;
    sendto(sock,error,5,0,(const struct sockaddr*) &addr,sizeof(addr));
    
}

int getBlockNum(char * buffer){
    unsigned short int * opcode_ptr;
    opcode_ptr = (unsigned short int *)buffer;
    return  ntohs(*(opcode_ptr + 1));
}

void Read(char* file, struct sockaddr_in addr, int TID){
    
    int sock;
    FILE* fp;
    //ssize_t n;
    int block=1;
    int readlen=BUF_LEN;   

    //initialize a socket for requests(client)
    sock = create_addr(TID);

    //get filename from the buffer and open
    fp=fopen(file+2,"rb");
    if(fp==NULL)
    {
        printf("error");
        Error(sock,1,addr);
        exit(1);
    }    
    //read from file
    //set the opcode to "data" and start sending packets
    //every loop is 1 package delivered and received ACK
    while(readlen == BUF_LEN)
    {
        char buffer1[MAX_BUF];
        buffer1[0] = '\0';
        buffer1[1] = '\3';             
        buffer1[2] = (char)(block>>8);
        buffer1[3] = (char)block;

       // opcode_ptr = (unsigned short int *)buffer1;
        //opcode = ntohs(*opcode_ptr);

        readlen = fread(buffer1+4, sizeof(char), BUF_LEN, fp);

        //*opcode_ptr = htons(DATA);
        //*(opcode_ptr+1) = htons(block);
      
        sender(sock,buffer1,readlen+4, addr);
        
        char ack[4];
        socklen_t len = sizeof(addr);
        recvfrom(sock,ack,4,MSG_WAITALL,(struct sockaddr *) &addr,&len);     
       /* 
        if(opcode==ACK){
            if(*(opcode_ptr + 1)!=block){
                
                Error(sock, 0,addr);
                perror("Wrong block#");
                exit(1);
            }

        }
        */
        block++;
    }
    close(sock);
    exit(0);
    
}

//hhr 


void Write(char* file, struct sockaddr_in addr,int TID){
    int sock;
    FILE* fp;
    ssize_t n;
    int block = 0;
    int blocknumber;
   
    char ack[4];
    //char ack2[4];
    
    
    int current_blocknumber = 1;
    //initialize a socket for requests(client)
    sock =create_addr(TID);

    //get filename from the buffer and open
    fp=fopen(file+2,"wb");
    if(fp==NULL){
        Error(sock,1,addr);
        exit(1);
    }
    
    //send ACK back
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
        //printf("do\n");
        //write into the file
        socklen_t len = sizeof(addr);
        n = recvfrom(sock,buffer,2048,MSG_WAITALL,(struct sockaddr *) &addr,&len);
        fflush(stdout);
        opcode_ptr = (unsigned short int *)buffer;
        opcode = ntohs(*opcode_ptr); 
        if (opcode != DATA){
            Error(sock,4,addr);
            exit(1);
        } 
        //block = ntohs(*(opcode_ptr+1));
        
        // char ack[4];
        // ack[0] = 0;
        // ack[1] = 4;
        // //opcode_ptr_ack = (unsigned short int *)ack2;
        // //blocknumber = (buffer[2] << 8)|buffer[3];
        // blocknumber = *((unsigned char*) buffer+3) | *((unsigned char*) buffer+2)<<8;
        //*opcode_ptr_ack = htons(DATA);
        //*(opcode_ptr_ack+1) = htons(blocknumber);
        //hhr
        blocknumber = getBlockNum(buffer);
        char ack[4];
        ack[0] = '\0';
        ack[1] = '\4';
        ack[2] = (char)blocknumber>>8;
        ack[3] = (char)blocknumber;

       // printf("block number: %d\n",blocknumber);
        
        
        // ack[2] = ((unsigned int)buffer[2]);
        // ack[3] = ((unsigned int)buffer[3]);

        if(blocknumber == current_blocknumber){
            fwrite( buffer+4,sizeof(char), n-4,fp);
            current_blocknumber++;
        }

        int bytesSend;
        bytesSend = sendto(sock,ack,4,0,(const struct sockaddr *) &addr,sizeof(addr));
        //printf("doend\n");
    }
    while(n==MAX_BUF);
    close(sock);
    fclose(fp);
    alarm(0);
    exit(0);
}

int main(int argc, char*argv[]){

    int port_lower = atoi(argv[1]);
    int port_upper = atoi(argv[2]);
   /* if(port_lower>=port_upper){
        perror("Port boundry");
        exit(1);
    }*/
    
    struct sockaddr_in client_sock_addr;
   
    //assigning the port lowerbound for destination socket 
    //and highest bound for server port 
    //to recieve the first request
    

    int TID = port_upper;
    char buffer[MAX_BUF];
    ssize_t n=0;
    int server;
    

    //initialize a socket for server
    server = create_addr(port_lower);

    signal(SIGCHLD, Sig_Child);


    while(1)
    {
        pid_t pid;
        socklen_t addr_len = sizeof(client_sock_addr);
        //client addr
        //struct sockaddr_in sock_addr;
        //getting the first request
        n = recvfrom(server, buffer, 2048, MSG_WAITALL,(struct sockaddr *)&client_sock_addr, &addr_len);
        

        //get opcode
        opcode_ptr = (unsigned short int *)buffer;
        opcode = ntohs(*opcode_ptr);
        // Illegal opcode;send out the error msg
        if(opcode != RRQ && opcode != WRQ) {

            Error(server, 4,client_sock_addr);
            exit(1);
        }

        //handle RRQ and WRQ 
        if(buffer[1] == RRQ){
            pid=fork();
            if(pid==0){        
                  
                Read(buffer,client_sock_addr,TID);
                //change port to the next highest                
                exit(0);
            }
        }
        if(buffer[1] == WRQ){

            pid=fork();
            if(pid==0){               
                Write(buffer,client_sock_addr,TID);        
                //change port to the next highest                
                exit(0);
            }
        }
        if (TID != port_lower){
            TID --;
        }

    }
    return 0;
}