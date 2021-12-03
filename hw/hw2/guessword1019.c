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
#include <stdbool.h>

#define MAX_BUF 2048
#define MAXLINE 2048

struct clientdata
{
    unsigned int fd;
    char username[MAXLINE];
    char word[MAXLINE];
};


// bool isfound(char charc, char* samech){
//   bool found = false;
//   for(int k = 0; k < sizeof(samech); k++){
//     if(charc == samch[k]){
//       found = true;
//     }
//   }
//   return found;
// }
//
// void append(char c, char s) {
//     int len = strlen(s);
//     s[len] = c;
//     s[len+1] = '\0';
// }
//
//
//
// int compareword(char* secretword, char client_word)
// {
//   if(strlen(client_word) != sizeof(secretword)){
//     send
//   }
//   char * samechar[sizeof(dictionary[word_number])];
//   int count = 0;
// 	int i,j;
// 	int samecharacter = 0;
// 	int samelocation = 0;
// 	int sameword = 0;
//
// 	for(i=0; i < sizeof(dictionary[word_number]); i++)
// 	{
// 		for(j=0; j < sizeof(dictionary[word_number]); j++)
// 		{
//
// 			if(buf[j] == dictionary[word_number][i] && !isfound(buf[j], samechar)) //这里没考虑转化成lowercase
// 			{
// 				samechar[count] = dictionary[word_number][i];
//         		count++;
// 				samecharacter += 1;
// 				if(j == i)
// 				{
// 					samelocation += 1;
// 				}
// 			}
// 		}
// 	}
//
// 	if(strcmp(buf,dictionary_lowercase) == 0)
// 	{
// 		sameword = 1;
// 	}
//
// 	*same_character1 = samecharacter;
// 	*same_location1 = samelocation;
// 	*same1 = sameword;
//
// }

// int comp(const void*a,const void*b) //qsort()
// {
// 	return *(int*)a-*(int*)b;
// }

int create_addr(int port){                  //create TCP socket
  int listenfd;
  // create socket
  if((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){ //zheng
      perror("Socket");
      return -1;
  }

  struct sockaddr_in ser_addr;
  bzero(&ser_addr, sizeof(ser_addr));
  // socklen_t len = sizeof(sock_addr);
  // memset(&sock_addr, 0, len);

  ser_addr.sin_family = PF_INET;
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //zheng
  ser_addr.sin_port = htons(port);

  if (bind(listenfd, (const struct sockaddr *) &ser_addr,sizeof(ser_addr)) < 0){
      perror("Binding");
      return -1;
  }

  //getsockname(sock, (struct sockaddr *) &ser_addr, & sizeof(ser_addr));
  //fflush(stdout);
  return listenfd;
}
//helper function to check whether fd set is empty.
bool FD_IS_ANY_SET(fd_set const *fdset)
{
    static fd_set empty;     // initialized to 0 -> empty
    return memcmp(fdset, &empty, sizeof(fd_set)) != 0;
}

int main(int argc, char*argv[])
{

  int maxfd, i, nready, connfd, n;
  int lines = 0;
  char ch;
  fd_set rset,allset;

  char buf[MAXLINE];
  // char bufname[5][MAXLINE];
  // char bufword[5][MAXLINE];

  struct sockaddr_in	cliaddr;
  socklen_t			clilen;
  struct clientdata client[5];


  int seed = atoi(argv[1]);
  int port_number = atoi(argv[2]);
  int longest_word_length = atoi(argv[4]); //read arguments
  char secretword[longest_word_length];
  int word_number;

  //read and initialize the dictionary
  FILE *f = fopen(argv[3], "rb");
  // fseek(f, 0, SEEK_END);
  // long fsize = ftell(f);
  // fseek(f, 0, SEEK_SET);

  //count the number of lines.
  while(!feof(f))
  {
    ch = fgetc(f);
    if(ch == '\n')
    {
      lines++;
    }
  }

  //define and dynamic allocate memory for dictionary.
  char dictionary[lines][longest_word_length];
  // for(int i = 0; i < lines; i++){
  //   dictionary[i] = malloc(longest_word_length*sizeof(char));
  // }
  //char dictionary_lowercase = malloc(fsize + 1);
  // char *dictionary_lowercase[lines];

  for(int i = 0; i < lines; i++)
  {
    if (fgets(dictionary[i], longest_word_length, f)) {
      printf("!!!!!!!!!!!!%s\n",dictionary[i]);
      break;
    }
    if (fgets(dictionary[i], longest_word_length, f) == NULL) break;
  }
  fclose(f);
  for(int i = 0; i < 50; i++)
  {
    printf("%s", dictionary[i]);
  }

  //change from upper case to lower case.

  // for(i = 0; i < line; i++)
  // {
  //
	// 	dictionary_lowercase[i] = tolower(dictionary[i]);
  //
  // }
  //
  //
  // qsort(dictionary_lowercase,fsize+1,longest_word_length,comp);

  //create TCP
  int serfd = create_addr(port_number);
  listen(serfd, 1024);

  //initialize the fd set.
	maxfd = serfd;
	for(i = 0;i < 5;i++)
	{
		client[i].fd = -1;
    strcpy(client[i].username, "");
    strcpy(client[i].word, "");
    // client[i].username = 0;
		// client[i].word = 0;
	}

	FD_ZERO(&allset);
	FD_SET(serfd, &allset);

	for( ; ; )
	{
    // if(!FD_IS_ANY_SET(&allset)){
    //    //creat secretnumber for secretword
    // }
	    srand(seed);
    	word_number = rand()% lines;
    	//printf("wordnumberis%d",word_number);
		rset = allset;

		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		// for(i=0;i<5;i++){
		// 	printf("maxfd%d",maxfd);
		// 	printf("nready%d",nready);
		// }

		if(FD_ISSET(serfd,&rset)) //case when new client in.
		{

      clilen = sizeof(cliaddr);
			connfd = accept(serfd, (struct sockaddr *) &cliaddr,& clilen);

		printf("CONNFD%d\n",connfd);
		printf("第一次值%d\n",client[0].fd);

     	 if(send(connfd, "Welcome to Guess the Word, please enter your username.\n", 54, 0) != 54){

               perror("send");
     	 }
     	 printf("第二次值%d\n",client[0].fd);
  		for(i = 0;i < 5;i++)
  		{
  			 printf("第三次值%d\n",client[0].fd);
  			if(client[i].fd == -1)
  			{
  				printf("第四次值%d\n",client[0].fd);
  				client[i].fd = connfd;
  				printf("第五次值%d\n",client[0].fd);
  				break;
  			}
  			//if(i == 5)
  			//err_quit("Too many clients");

  		}
        FD_SET(connfd, &allset);
        if (connfd > maxfd){
        	maxfd = connfd;
        }
  		if (--nready <= 0){
  			continue;
  		}

    }


    /* check all clients for data */
    for (i = 0; i < 5; i++) {
    	//printf("ww\n");
    		int clifd;


			if ( (clifd = client[i].fd) ==-1){
				//printf("这个%d\n",clifd);
				continue;
			}

			if (FD_ISSET(clifd, &rset)) {


        //if disconnected, close the socket.
        username_again:

				if ( (n = read(client[i].fd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
					close(client[i].fd);
					FD_CLR(client[i].fd, &allset);
          client[i].fd = -1;
          // memset(client[i].username, 0, sizeof(client[i].username));
          // memset(client[i].word, 0, sizeof(client[i].word));
          strcpy(client[i].username, "");
          strcpy(client[i].word, "");
				}
        //read from client[i]
        else if(n > 0)
        {
        	printf("nis%s\n",dictionary[1]);
          //first we need to read the username.
          if(strcmp(client[i].username,"") == 0)
          {
            printf("检测1\n");
            //change from uppper case to lower case.
            for(int k = 0; k < strlen(buf)-1; k++) //sizeof could
            {
            	printf("检测2\n");

                buf[k] = tolower(buf[k]);

            }
			printf("检测3\n");
            for(int j = 0;j < 5;j++)
            {
            	printf("这是%s\n",client[j].username);
            	printf("那是%s\n", buf);
            	printf("名字长度%ld\n",strlen(client[0].username));
            	printf("第二个名字长度%ld\n",strlen(buf));
              if(strcmp(client[j].username, buf) == 0)
              {
                send(client[i].fd, "Username is already taken, please enter a different username\n", 60, 0);
                //clear buffer
                memset(buf, 0, MAXLINE);
                goto username_again;
              }
            }
            printf("检测4\n");

            //assign content on buffer to client.


            strncpy(client[i].username, buf, strlen(buf));

            //strncpy(client[i].username, buf2, strlen(buf2));

            //printf("username%s\n",client[i].username);

            strcat(buf,": Let's start playing, ");
            send(client[i].fd, buf, strlen(buf),0);
            send(client[i].fd, client[i].username, sizeof(client[i].username), 0);
            memset(buf,0,MAXLINE);
             printf("检测5\n");
          }

          //then we need to read word.
          else
          {
            printf("检测6\n");
            strncpy(client[i].word, buf, strlen(buf));
            //strncpy(client[i].word, buf, strlen(buf));

            //case when guess word length not equal to secret word length.
            if(strlen(buf) != strlen(dictionary[word_number]))
            {

              memset(buf, 0, MAXLINE);
              printf("检测7\n");
              strcat(buf, "Invalid guess length. The secret word is ");
              printf("检测8\n");
              strcat(buf, dictionary[word_number]);
               printf("检测9\n");
              strcat(buf, " letter(s).");
               printf("检测10\n");
              send(client[i].fd, buf, strlen(buf),0);
               printf("检测11\n");
              memset(buf, 0, MAXLINE);
            }
            else
            {
              strcpy(secretword, dictionary[word_number]);

            	int samecharacter = 0;
            	int samelocation = 0;
            	int sameword = 0;
              //case to calculate the right word and right position.
            	for(int k = 0; k < strlen(secretword); k++)
            	{
            		for(int j=0; j < strlen(secretword); j++)
            		{

            			if(buf[j] == secretword[k])
            			{
                    secretword[k] = 0;
            				samecharacter++;
            				if(j == k)
            				{
            					samelocation++;
            				}
            			}
            		}
            	}
              //if client guess the word.
              if(samelocation == strlen(secretword))
              {
                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " has correctly guessed the word ");
                strcat(buf, secretword);

                for(int j = 0; j < 5; j++){
                  send(client[j].fd, buf, strlen(buf), 0);
                  close(client[j].fd);
                  FD_CLR(client[j].fd, &allset);
                  client[j].fd = -1;
                  // memset(client[i].username, 0, sizeof(client[i].username));
                  // memset(client[i].word, 0, sizeof(client[i].word));
                  strcpy(client[i].username, "");
                  strcpy(client[i].word, "");
                }
                break;
              }
              else
              {
              	char sameword_char[1], samelocation_char[1];

            		sameword_char[0] = sameword;
            		samelocation_char[0] = samelocation;

                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " guessed ");
                strcat(buf, client[i].word);
                strcat(buf, ": ");
                strcat(buf, sameword_char);
                strcat(buf, " letter(s) were correct and ");
                strcat(buf, samelocation_char);
                strcat(buf, " letter(s) were correctly placed.");
                for(int j = 0; j < 5; j++){
                  send(client[j].fd, buf, strlen(buf), 0);
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
