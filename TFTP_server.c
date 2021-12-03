#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>



#define RETRIES 10
#define BUF_LEN 512

unsigned short int opCode;
unsigned short int * opCode_ptr;

int min, max, tid;


enum opCode {
     RRQ=1,
     WRQ,
     DATA,
     ACK,
     ERROR
};

//structure for messages
typedef union {

     uint16_t opCode;
    //RRQ and WRQ structure
     struct {
          uint16_t opCode;
          uint8_t filename[514];
     } request;
    //DATA structure
     struct {
          uint16_t opCode;
          uint16_t number;
          uint8_t data[512];
     } data;
    //ACK structure
     struct {
          uint16_t opCode;
          uint16_t number;
     } ack;
    //ERROR structure
     struct {
          uint16_t opCode;
          uint16_t code;
          uint8_t string[513];
     } error;

} message_pack;

void sigchild(int sig){
  pid_t pid;
  int stat;
  While((pid = waidpid(-1, &stat, WNOHANG)) > 0){
    printf("Parent watches Child %d terminated with status %d.\n",pid,stat)
  }
}

ssize_t sendError(int s, int code, char *info,
                        struct sockaddr_in *sock, socklen_t slen)
{
     message_pack m;
     ssize_t c;

     if(strlen(info) >= 512) {
          fprintf(stderr, "server: error string too long\n");
          return -1;
     }

     m.opCode = htons(ERROR);
     m.error.code = code;
     strcpy(m.error.string, info);

     if ((c = sendto(s, &m, 4 + strlen(info) + 1, 0,
                     (struct sockaddr *) sock, slen)) < 0) {
          perror("server: sendto()");
     }

     return c;
}

int create_addr(int port){

}

void handler(char* filename, struct sockaddr_in *client_sock,int tid)
{


    //open new socket for client
    // if ((s = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
    //     perror("server: socket()");
    //     exit(1);
    // }


    //set socket's timeout values
    //tv.tv_sec  = TIMEOUT;
    //tv.tv_usec = 0;

    // if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    //     perror("setsockopt failed\n");

    // if (setsockopt (s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
    //     perror("setsockopt failed\n");

    // Get filename and opCode from message_pack struct




    // Read request handler
    if(filename[1]==RRQ)
    {
      int sock;
      FILE *fp;
      // create socket
      if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
          perror("Socket");
          return -1;
      }

      struct sockaddr_in client_sock;
      socklen_t len = sizeof(client_sock);
      memset(&client_sock, 0, len);

      client_sock.sin_family = AF_INET;
      client_sock.sin_addr.s_addr = INADDR_ANY;
      client_sock.sin_port = htons(port);

      if (bind(sock, (const struct sockaddr *) &client_sock,len) < 0){
          perror("Binding");
          return -1;
      }

      getsockname(sock, (struct sockaddr *) &client_sock, &len);
      fflush(stdout);

      fp = fopen(filename+2,"rb");
      if(fp == NULL)
      {
          exit(1);
      }

        // if( access( filename, F_OK ) == -1 ) {
        //     //file does not exist
        //     perror(filename);
        //     sendError(s,htons(1),"File not found",client_sock,slen);
        //     exit(1);
        // }
        // else {
        //     //open file for reading
        //     file = fopen(filename, "r");
        //     if (file == NULL) {
        //         perror("server: fopen()");
        //         sendError(s, htons(0), strerror(errno), client_sock, slen);
        //         exit(1);
        //     }
            //read the file in 512 byte chunks
            int blockNum=1;

            int needClose=0;
            // This loop will run as long as there 512 bytes to be read
            while(needClose==0)
            {
                int count;
                ssize_t read_len=0;
                //ssize_t n=0;

                uint8_t read[516];

                for(int i = 0; i<516;i++){
                    read[i]='\0';
                }
                // Store data in a buffer
                read_len = fread(read+4,1,BUF_LEN,fp);


                // Check if we will need to transfer more packets
                if(read_len<BUF_LEN){
                    needClose=1;
                }

                //try 10 times to send the data
                // for(count=RETRIES;count;count--){
                //     message_pack hold;
                //     struct sockaddr_in hold_socket;
                //     hold.opCode = htons(DATA);
                //     hold.data.number = htons(blockNum);
                //     for(int i = 0; i<512;i++){
                //         hold.data.data[i]='\0';
                //     }

                //     memmove(hold.data.data, read, read_len);
                printf("到这里了1");
                    //send data
                    int n;
                    n = sendto(sock, read, read_len+4, 0, (struct sockaddr *) & client_sock, sizeof(struct sockaddr_in));
                    if(n<0){
                        perror("sendto");
                        printf("到这里了2");
                        exit(1);
                    }
                    printf("到这里了3");

                    //recieve ack
                    char ack[4];
                    socklen_t len = sizeof(client_sock);
                    n=recvfrom(sock,ack,4,0,(struct sockaddr *) client_sock,&len);
                    blockNum+=1;
                    //recieved invalid message_pack
                    // if (n>=0 && n<4){
                    //     exit(1);
                    // }

                    // // Break when we recieve a valid message_pack
                    // if(n>=4){
                    //     n=0;
                    //     break;
                    // }
                }

                // Check if we ran out of transfer retries
                // if (!count){
                //     printf("transfer timed out\n");
                //     exit(1);
                // }

                // if(ntohs(m2.opCode)!=ACK){
                //     printf("recieved invalid message_pack during transfer\n");
                //     exit(1);
                // }

                // if(ntohs(m2.ack.number)!=blockNum){
                //     printf("recieved invalid ack number\n");
                //     sendError(s,htons(0),"Invalid ACK number",client_sock,slen);
                //     exit(1);
                // }
                // if(ntohs(m2.opCode)==ERROR){
                //     printf("recieved error message_pack from client");
                //     exit(1);
                // }

    }

    // Write request handler
    // else if(file[1] ==WRQ){
    // // Check if the file already exists
    //     if( access( filename, F_OK ) != -1 ) {
    //         sendError(s,htons(6),"File already exists",client_sock,slen);
    //         exit(1);
    //     }
    //     else {
    //         file = fopen(filename, "w");
    //         if (file == NULL) {
    //             perror("server: fopen()");
    //             sendError(s, htons(0), strerror(errno), client_sock, slen);
    //             exit(1);
    //         }

    //         //read the file in 512 byte chunks
    //         uint16_t blockNum=0;
    //         message_pack a;
    //         a.opCode=htons(ACK);
    //         a.ack.number=htons(blockNum);
    //         ssize_t n=0;

    //         //send initial ack message_pack
    //         n=sendto(s,&a,sizeof(a.ack),0,(struct sockaddr *) client_sock,slen);
    //         if(n<0){
    //             printf("transfer killed\n");
    //             exit(1);
    //         }

    //         // Run the loop while there is still data that needs to be transferred
    //         int needClose=0;
    //         while(needClose==0){
    //             int count;
    //             message_pack m2;

    //             //try 10 times to send the ack
    //             for(count=RETRIES; count;count--){
    //                 printf("%d\n",count );
    //                 struct sockaddr_in hold_socket;

    //                 //recieved data
    //                 n=recvfrom(s,&m2,sizeof(m2),0,(struct sockaddr *) &hold_socket,&slen);
    //                 if(n>=0&&n<4){
    //                     //recieved invalid message_pack
    //                     sendError(s, htons(0), "Invalid request size", client_sock, slen);
    //                     exit(1);
    //                 }

    //                 // Break when we recieve a valid message_pack
    //                 if (n>=4) {
    //                     break;
    //                 }
    //                 //send ack
    //                 a.ack.number=htons(blockNum);
    //                 n=sendto(s,&a,sizeof(a.ack),0,(struct sockaddr *) client_sock,slen);
    //                 if(n<0){
    //                     printf("transfer killed\n");
    //                     exit(1);
    //                 }
    //             }

    //             if(!count){
    //                 printf("transfer timed out\n");
    //                 exit(1);
    //             }
    //             blockNum++;
    //             // Break when we are done recieving data
    //             if (strlen(m2.data.data)<512){
    //                 needClose=1;
    //             }
    //             if(ntohs(m2.opCode)==ERROR){
    //                 printf("Recieved ERROR message_pack\n");
    //                 exit(1);
    //             }
    //             if(ntohs(m2.opCode)!=DATA){
    //                 printf("Recieved incorrect message_pack\n");
    //                 sendError(s,htons(0),"Incorrect message_pack passed during transfer",client_sock, slen);
    //             }
    //             if (ntohs(m2.ack.number) != blockNum) {
    //                 printf("Invalid block number\n");
    //                 sendError(s,htons(0),"Invalid block number\n",client_sock, slen);
    //             }
    //             //write data to file
    //             n = fwrite(m2.data.data, 1, n - 4, file);
    //             if(n<0){
    //                 perror("fwrite failed");
    //                 exit(1);
    //             }
    //             a.ack.number=htons(blockNum);
    //             n=sendto(s,&a,sizeof(a.ack),0,(struct sockaddr *) client_sock,slen);

    //             if (n < 0) {
    //                 printf("transfer killed\n");
    //                 exit(1);
    //             }
    //         }
    //         printf("Transfer Finished\n");
    //         fclose(file);
    //         close(s);
    //     }
    // }
}

int main(int argc, char *argv[]){
    // if (argc != 3 || atoi(argv[1]) < 1024 || atoi(argv[2]) > 65535) {
    //  perror("Usage: ./server port[1] port[2]. port[1] should be greater than 1024, port[2] should "
    //      "be smaller than 65535.");
    //  exit(-1);
    // }

    int port_min = atoi(argv[1]);
    int port_max = atoi(argv[2]);
    int tid = port_max;
    //int port = atoi(argv[1]);
    //tid = port + 1;

    ssize_t n;
    //char buffer[BUF_LEN];
    char buffer[1024];
    int server_socket;
    signal(SIGCHLD, sigchild);

    if((server_socket = socket(PF_INET, SOCK_DGRAM, 0))<0){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    memset(&server, 0, server_len);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port_min);



    if(bind(server_socket, (struct sockaddr *)&server, server_len)<0){
        perror("bind");
        exit(-1);
    }

    getsockname(server_socket, (struct sockaddr *)&server, &server_len);

    printf("Begin TFTP Server on port: %d\n", ntohs(server.sin_port));

    printf("???");
    while(1){
        pid_t pid;
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);

        n = recvfrom(server_socket,buffer, 1024, 0, (struct sockaddr *) &client, &client_len);
        int i;
        for(i=0;i<8;i++){
          printf("%s",buffer[i]);
        }
        if(n < 0) {
            perror("recvfrom");
            exit(-1);
        }


        opCode_ptr = (unsigned short int *)buffer;
        opCode = ntohs(*opCode_ptr);

        //opCode = ntohs(mp.opCode);

        if(opCode != RRQ && opCode != WRQ)
        {
            char error[2];
            error[0] = '\0';
            error[1] = '\5';
            n = sendto(server_socket, error, 2, 0, (struct sockaddr *)&client, client_len);
        //     *opCode_ptr = htons(ERROR);
        //     *(opCode_ptr + 1) = htons(4);
        //     *(buffer + 4) = 0;

        // intr_send:
        //     n = sendto(server_socket, buffer, 5, 0, (struct sockaddr *)&server, server_len);
        //     if(n < 0)
        //     {
        //         if(errno == EINTR) goto intr_send;
        //         perror("sendto");
        //         exit(-1);
        //     }
        }
        else{
            if((pid = fork()) == 0){
              handler(buffer, &client,tid);
              exit(0);
            }
        }

        if (tid != port_min){
            tid --;
        }


    }
    return 0;
}
