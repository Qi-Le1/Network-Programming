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

int main(){
  char **where_recv_senddata1;
	where_recv_senddata1 = malloc(50 * sizeof (char *));

	int i;
	for(i = 0; i < 50; i++)
	{
		where_recv_senddata1[i] = malloc(40 * sizeof (char ));
	}
  for(i=0;i<50;i++)
{
  free(where_recv_senddata1[i]);
}
free(where_recv_senddata1);
}
