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
  fd_set rset,allset;

  char buf[MAXLINE];
  char minbuf[MAXLINE];
  // char bufname[5][MAXLINE];
  // char bufword[5][MAXLINE];

  struct sockaddr_in	cliaddr;
  socklen_t			clilen;
  struct clientdata client[5];


  int seed = atoi(argv[1]);

  bool isover = true;
  int port_number = atoi(argv[2]);
  int longest_word_length = atoi(argv[4]); //read arguments
  char secretword[longest_word_length];
  char cpy_secretword[longest_word_length];
  char secretword_char[60];
  int word_number;



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

  srand(seed);
	for( ; ; )
	{
    if(isover == true){
      FILE *f = fopen(argv[3], "rb");
      word_number = rand()%499326;
      printf("%d\n", word_number);
      //read and initialize the dictionary

      int count = 0;
      while( (fgets(secretword, longest_word_length, f) )!= NULL)
      {
          if(count == word_number){
            for(int g = 0; g < strlen(secretword); g++)
            {
              if(secretword[g] == '\n')
              {
                secretword[g] = '\0';
              }
            }
            fclose(f);
            break;
          }
          count++;
      }
      sprintf(secretword_char, "%ld", strlen(secretword));
    }

		rset = allset;

		nready = select(maxfd+1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(serfd,&rset)) //case when new client in.
		{

      clilen = sizeof(cliaddr);
			connfd = accept(serfd, (struct sockaddr *) &cliaddr,& clilen);
      isover = false;
     	 if(send(connfd, "Welcome to Guess the Word, please enter your username.\n", 54, 0) != 54){
               perror("send");
     	 }
  		for(i = 0;i < 5;i++)
  		{
  			if(client[i].fd == -1)
  			{
  				client[i].fd = connfd;
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
    	int clifd;
			if ( (clifd = client[i].fd) ==-1){
				continue;
			}
			if (FD_ISSET(clifd, &rset)) {


        //if disconnected, close the socket.
        username_again:

				if ( (n = read(client[i].fd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
            strcpy(client[i].username, "");
            strcpy(client[i].word, "");
					close(client[i].fd);
					FD_CLR(client[i].fd, &allset);
          client[i].fd = -1;
          // memset(client[i].username, 0, sizeof(client[i].username));
          // memset(client[i].word, 0, sizeof(client[i].word));

          isover = true;
				}
        //read from client[i]
        else if(n > 0)
        {
          for(int g = 0; g < strlen(buf); g++){
            if(buf[g] == '\n'){
              buf[g] = '\0';
            }
          }
          isover = false;
          //first we need to read the username.
          if(strcmp(client[i].username,"") == 0)
          {
            //change from uppper case to lower case.
            for(int k = 0; k < strlen(buf); k++) //sizeof could
            {
                buf[k] = tolower(buf[k]);
            }
            for(int j = 0;j < 5;j++)
            {
              if(strcmp(client[j].username, buf) == 0)
              {
                send(client[i].fd, "Username is already taken, please enter a different username\n", 60, 0);
                //clear buffer
                memset(buf, 0, MAXLINE);
                goto username_again;
              }
            }

            //assign content on buffer to client.
            //simple loop to check the connection number.
            int clinum = 0;
            for (int q = 0; q < 5; q++)
            {
        			if (client[q].fd !=-1)
              {
        				clinum++;
        			}
            }
            char clinum_char[60];
            sprintf(clinum_char, "%d", clinum);
            strncpy(client[i].username, buf, strlen(buf));
            memset(buf, 0, MAXLINE);
            strcat(buf,"Let's start playing, ");
            strcat(buf, client[i].username);
            strcat(buf, "\n");
            strcat(buf, "There are ");
            strcat(buf, clinum_char);
            strcat(buf, " player(s) playing. The secret word is ");
            strcat(buf, secretword_char);
            strcat(buf, " letter(s).");
            strcat(buf, "\n");
            send(client[i].fd, buf, strlen(buf),0);
            memset(buf, 0, MAXLINE);
          }

          //then we need to read word.
          else
          {

            strncpy(client[i].word, buf, strlen(buf));
            //case when guess word length not equal to secret word length.
            if(strlen(buf) != strlen(secretword))
            {

              memset(buf, 0, MAXLINE);
              memset(client[i].word, 0, MAXLINE);
              strcat(buf, "Invalid guess length. The secret word is ");
              strcat(buf, secretword_char);
              strcat(buf, " letter(s).");
              strcat(buf, "\n");
              send(client[i].fd, buf, strlen(buf),0);
              memset(buf, 0, MAXLINE);
            }
            //read and judge the word
            else
            {
              memset(minbuf, 0, MAXLINE);
              strcpy(cpy_secretword, secretword);
              strcpy(minbuf, buf);
              printf("%ld\n",strlen(cpy_secretword));

            	int samecharacter1 = 0;
              int samecharacter2 = 0;
              int samecharacter = 0;
            	int samelocation = 0;
              //case to calculate the right word and right position.
            	for(int k = 0; k < strlen(secretword); k++)
            	{
            		for(int j=0; j < strlen(secretword); j++)
            		{
            			if(client[i].word[j] == secretword[k])
            			{
                    if(minbuf[j] != '*')
                    {
                      minbuf[j] = '*';
              				//samecharacter++;
                    }
                    if(cpy_secretword[k] != '*'){
                      cpy_secretword[k] = '*';
                    }
            				if(j == k)
            				{
            					samelocation++;
            				}
            			}
            		}
            	}
              printf("%d\n", samelocation);
              for(int k = 0; k < strlen(secretword); k++){
                if(minbuf[k] == '*'){
                  samecharacter1++;
                }
              }
              for(int k = 0; k < strlen(secretword); k++){
                if(cpy_secretword[k] == '*'){
                  samecharacter2++;
                }
              }
              if(samecharacter2 <= samecharacter1){
                samecharacter = samecharacter2;
              }else{
                samecharacter = samecharacter1;
              }

              // memset(cpy_secretword, 0, MAXLINE);
              //if client guess the word.
              if(samelocation == strlen(secretword))
              {
                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " has correctly guessed the word ");
                strcat(buf, secretword);

                for(int j = 0; j < 5; j++){
                  if(client[j].fd != -1){
                    send(client[j].fd, buf, strlen(buf), 0);
                    strcpy(client[j].username, "");
                    strcpy(client[j].word, "");
                    close(client[j].fd);
                    client[j].fd = -1;
                    FD_CLR(client[j].fd, &allset);

                  }
                }
                isover = true;
                memset(buf, 0, MAXLINE);
              }
              else
              {

                char sameword_char[60];
                char samelocation_char[60];
                sprintf(sameword_char, "%d", samecharacter);
                sprintf(samelocation_char, "%d", samelocation);



                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " guessed ");
                strcat(buf, client[i].word);
                strcat(buf, ": ");
                strcat(buf, sameword_char);
                strcat(buf, " letter(s) were correct and ");
                strcat(buf, samelocation_char);
                strcat(buf, " letter(s) were correctly placed.");
                strcat(buf, "\n");
                memset(client[i].word, 0, MAXLINE);
                for(int j = 0; j < 5; j++){
                  send(client[j].fd, buf, strlen(buf), 0);
                }
                memset(buf, 0, MAXLINE);
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
