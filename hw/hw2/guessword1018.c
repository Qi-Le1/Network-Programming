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
  if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){ //zheng
      perror("Socket");
      return -1;
  }

  struct sockaddr_in ser_addr;
  bzero(&ser_addr, sizeof(ser_addr))
  // socklen_t len = sizeof(sock_addr);
  // memset(&sock_addr, 0, len);

  ser_addr.sin_family = AF_INET;
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //zheng
  ser_addr.sin_port = htons(SERV_PORT);

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

  int maxfd, clifd;
  int lines = 0;
  bool found;
  fd_set rset,allset;

  char buf[MAXLINE];
  struct sockaddr_in	cliaddr;
  struct clientdata client[5];


  int seed = atoi(argv[1]);
  int port_number = atoi(argv[2]);
  int longest_word_length = atoi(argv[4]); //read arguments

  //read and initialize the dictionary
  FILE *f = fopen(argv[3], "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

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
  char *dictionary[lines];
  // for(int i = 0; i < lines; i++){
  //   dictionary[i] = malloc(longest_word_length*sizeof(char));
  // }
  //char dictionary_lowercase = malloc(fsize + 1);
  // char *dictionary_lowercase[lines];

  for(int i = 0; i < lines; i++)
  {
    if (fgets(dictionary[i], longest_word_length, f) == NULL) break;
  }
  fclose(f);

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
  int serfd = create_addr();
  Listen(serfd, LISTENQ);

  //initialize the fd set.
	maxfd = serfd;
	for(i = 0;i < 5;i++)
	{
		client[i].fd = -1;
    client[i].username = 0;
		client[i].word = 0;
	}

	FD_ZERO(&allset);
	FD_SET(serfd, &allset);

	for( ; ; )
	{
    if(!FD_IS_ANY_SET()){
      srand(seed);
    	int word_number = rand()% fsize;   //creat secretnumber for secretword
    }

		rset = allset;
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(serfd,&rest)) //case when new client in.
		{
			connfd = Accept(serfd, (SA *) &cliaddr, & sizeof(cliaddr));
      if(send(connfd, "Welcome to Guess the Word, please enter your username.\n", 54, 0) != 54)
               perror("send");
  		for(i = 0;i < 5;i++)
  		{
  			if(client[i].fd < 0)
  			{
  				client[i].fd = connfd;
  				break;
  			}
  			//if(i == 5)
  			//err_quit("Too many clients");

        FD_SET(connfd, &allset);
        if (connfd > maxfd)
  				maxfd = connfd;			/* for select */
  			if (--nready <= 0)
  				continue;
  		}
    }


    /* check all clients for data */
    for (i = 0; i < 5; i++) {
			if ( (clifd = client[i].fd) < 0)
				continue;

			if (FD_ISSET(client[i].fd, &rset)) {
        //if disconnected, close the socket.
        username_again:
				if ( (n = Read(client[i].fd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
					Close(client[i].fd);
					FD_CLR(client[i].fd, &allset);
          client[i].fd = -1;
          client[i].username = 0;
          client[i].word = 0;
				}
        //read from client[i]
        else if(n > 0)
        {
          //first we need to read the username.
          if(client[i].username == 0)
          {
            //change from uppper case to lower case.
            for(int k = 0; k < strlen(buf); k++) //sizeof could
            {
              while(buf[k]){
                buf[k] = tolower(buf[k]);
              }
            }

            for(int j = 0;j < 5;j++)
            {
              if(client[j].username == buf)
              {
                send(client[i].fd, "Username is already taken, please enter a different username\n", 54, 0);
                //clear buffer
                memset(buf, 0, MAXLINE);
                goto username_again;
              }
            }
            //assign content on buffer to client.
            strncpy(client[i].username, buf, strlen(buf));
            strcat(buf, ", Let's start playing, ");
            send(client[i].fd, buf, strlen(buf) 0);
            send(client[i].fd, client[i].username, sizeof(client[i].username), 0);
          }

          //then we need to read word.
          else
          {
            strncpy(client[i].word, buf, strlen(buf));
            //case when guess word length not equal to secret word length.
            if(strlen(buf) != sizeof(dictionary[word_number]))
            {
              memset(buf, 0, MAXLINE);
              strcat(buf, "Invalid guess length. The secret word is ");
              strcat(buf, dictionary[word_number]);
              strcat(buf, " letter(s).");
              send(client[i].fd, buf, strlen(buf) 0);
              memset(buf, 0, MAXLINE);
            }
            else
            {
              char samechar[sizeof(dictionary[word_number])];
              char* secretword;
              strcpy(secretword,dictionary[word_number]);
            	int samecharacter = 0;
            	int samelocation = 0;
            	int sameword = 0;
              //case to calculate the right word and right position.
            	for(int k = 0; k < sizeof(secretword); k++)
            	{
            		for(int j=0; j < sizeof(secretword); j++)
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
              if(samelocation == sizeof(secretword))
              {
                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " has correctly guessed the word ");
                strcat(buf, secretword);
                for(int j = 0; j < 5; j++){
                  send(client[j], buf, strlen(buf), 0);
                  close(client[j]);
                  FD_CLR(client[j].fd, &allset);
                  client[j].fd = -1;
                  client[j].username = 0;
                  client[j].word = 0;
                }
                break;
              }
              else
              {
                memset(buf, 0, MAXLINE);
                strcpy(buf, client[i].username);
                strcat(buf, " guessed ");
                strcat(buf, client[i].word);
                strcat(buf, ": ");
                strcat(buf, sameword);
                strcat(buf, " letter(s) were correct and ");
                strcat(buf, samelocation);
                strcat(buf, " letter(s) were correctly placed.");
                for(int j = 0; j < 5; j++){
                  send(client[j], buf, strlen(buf), 0);
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
