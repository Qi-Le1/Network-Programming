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
int main(){
  // char *
  // char ** buffer = calloc(5, sizeof(char));
  // char buf[100];
  // // int i = 0;
  // // // buf = 1;
  // // // printf("%ld",strlen(buf));
  // FILE* f = fopen("trial.txt", "r");
  // // //
  // // //
  // for(int i = 0; i < 3; i++){
  //   if (fgets(buf, 20, f) == NULL){
  //     break;
  //   }
  // }
  // //
  // //
  // //
  // //
  // for(int j = 0; j < 3; j++){
  //   printf("%s",buf[j]);
  // }
  char** j;
  j[0] = "lalla";
  printf("%s", j[0]);

  // char buf[2][20];
  // char buf1[20];
  // strcpy(buf[1], "hehhhhh");
  //
  // // char* buf1 = "hello";
  // strcpy(buf1, buf[1]);
  // // strcat(buf, buf1);
  // // strcpy(buf, buf1);
  // printf("%s ",buf1);
  // if(buf1 == buf){
  //   for(int i= 0; i < sizeof(buf); i++){
  //     printf("%c", buf[i]);
  //   }
  // }

  //char buf2[99];
  //buf = "Welcome to Guess the Word, please enter your username";

  //buf[1] = malloc(100*sizeof(char));
  //strcpy(buf, "Welcome to Guess the Word, please enter your username");
  //buf2 = buf;
  // strcpy(buf2, buf);

  // printf("%c", c);
  //printf("%ld", strlen(buf));
  // int length = sizeof(buffer)/sizeof(int);
  // char* buffer[10]; //sizeof(buffer) = 80
  // printf("%ld",sizeof(buffer));
  return 1;
}
