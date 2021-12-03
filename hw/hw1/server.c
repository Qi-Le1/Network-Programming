#include "../../unpv13e_mini/lib/unp.h"

#define BUFSIZE 512
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR 5

void sig_child(int signo);

typedef struct ack_struct_
{
    uint16_t opcode;
    uint16_t block_number;
} ack_struct;

typedef struct data_struct_
{
    uint16_t opcode;
    uint16_t block_number;
    char data[BUFSIZE];
} data_struct;

typedef struct error_struct_
{
    uint16_t opcode;
    uint16_t ErrorCode;
    char ErrorMsg[BUFSIZE];
} error_struct;

int main(int argc, char* argv[]) {
    if (argc != 3 || atoi(argv[1]) < 1024 || atoi(argv[2]) > 65535) {
        perror("Usage: ./server port[1] port[2]. port[1] should be greater than 1024, port[2] should "
               "be smaller than 65535.");
        exit(1);
    }
    int socket_fd;
    uint16_t block_number = 0;
    int port = atoi(argv[1]);
    ssize_t received = 0;
    ssize_t sent = 0;
    ssize_t read_size = 0;
    uint8_t send_buffer[BUFSIZE];
    memset(&send_buffer, 0, sizeof send_buffer);
    char receive_buffer[BUFSIZE];
    memset(receive_buffer, 0, sizeof receive_buffer);
    uint16_t opcode;
    char file[100];
    memset(file, 0, sizeof file);
    char new_file[1000];
    memset(new_file, 0, sizeof new_file);
    char directory[100];
    memset(directory, 0, sizeof directory);
    char mode[10];
    memset(mode, 0, sizeof mode);
    ack_struct ack_message;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof client_addr;
    socket_fd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&server_addr, sizeof server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    Bind(socket_fd, (SA *) &server_addr, sizeof server_addr);

//    Signal(SIGCHLD, sig_child);
    while(1) {
        received = Recvfrom(socket_fd, receive_buffer, BUFSIZE, 0, (SA *) &client_addr, &len);
        opcode = receive_buffer[1];

//        opcode = ntohs((buffer[0] << 8) | buffer[1]);
//        printf("%s\n", opcode);
        if (opcode == RRQ) {
            sprintf(file, "%s", receive_buffer+2);
            sprintf(mode, "%s", receive_buffer + 2 + strlen(file) + 1);
             if (strcmp(mode, "octet") != 0) {
                 perror("Mode is not octet.\n");
                 exit(1);
             }
             if (getcwd(directory, sizeof(directory)) != NULL) {
                 strcat(directory, "/");
                 strcat(directory, file);
                 printf("%s\n", directory);
             }
             else {
                 ////TODO
                 
                 fprintf(stderr, "%s @ %d: file directory wrong.\n", __FILE__, __LINE__);
                 exit(1);
             }
             int file_d = open(directory, O_RDONLY, S_IRUSR);
             if (file_d < 0) {
                 //TODO send error message
                 fprintf(stderr, "%s @ %d: file open failure.\n", __FILE__, __LINE__);
                 exit(1);
             }

             data_struct data_to_send;
             memset(&data_to_send, 0, sizeof data_to_send);
             data_to_send.opcode = htons(DATA);
             block_number = 1;
             data_to_send.block_number = htons(block_number);

             while ((read_size = read(file_d, data_to_send.data, BUFSIZE)) > 0) {
                 data_to_send.block_number = htons(block_number);
                 Sendto(socket_fd, &data_to_send, 4 + read_size, 0, (SA *) &client_addr, len);
                 memset(&ack_message, 0, sizeof ack_message);
                 Recvfrom(socket_fd, &ack_message, sizeof ack_message, 0, (SA*) &client_addr, &len);
                 if (ack_message.block_number != data_to_send.block_number) {
                     printf("Ack received not right.\n");
                     exit(1);
                 }
                 block_number++;
             }
        }

        else if (opcode == WRQ) {
            sprintf(file, "%s", receive_buffer + 2);
            sprintf(mode, "%s", receive_buffer + 2 + strlen(file) + 1);
            if (strcmp(mode, "octet") != 0) {
                perror("Mode is not octet.\n");
                exit(1);
            }
            if (getcwd(directory, sizeof directory) != NULL) {
                strcat(directory, "/");
                strcat(directory, file);
            } else {
                fprintf(stderr, "%s @ %d: file directory wrong.\n", __FILE__, __LINE__);
                exit(1);
            }
            int file_d = open(directory, O_RDWR | O_CREAT);
            if (file_d < 0) {
                fprintf(stdout, "%s @ %d: file open wrong\n", __FILE__, __LINE__);
                exit(1);
            }
            memset(&ack_message, 0, sizeof ack_message);

            block_number = 0;
            ack_message.opcode = htons(ACK);
            ack_message.block_number = htons(block_number);
            Sendto(socket_fd, &ack_message, sizeof ack_message, 0, (SA *) &client_addr, len);

            data_struct data_received;
            memset(&data_received, 0, sizeof data_received);
            while ((received = Recvfrom(socket_fd, &data_received, BUFSIZE, 0, (SA *) &client_addr, &len)) > 0) {
                if (ntohs(data_received.opcode) != DATA) {
                    printf("Data received with wrong opcode: %c\n", ntohs(data_received.opcode));
                    exit(1);
                }
                write(file_d, data_received.data, strlen(data_received.data));
                ack_message.block_number = data_received.block_number;
                Sendto(socket_fd, &ack_message, sizeof ack_message, 0, (SA *) &client_addr, len);
                memset(&data_received, 0, sizeof data_received);
            }
            close(file_d);
        }
        break;
    }
    return 0;
}