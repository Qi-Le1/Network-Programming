//client TCP
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>

//Finding the nearest next ID in SENDDATA
char* range(char I_D[30][30], char x_position[30][30], char y_position[30][30], int destinationx, int destinationy, int reachable_number)
	{
		int i = 0,j;
		int xposition,yposition,distance_number;
		int distance;
		distance = 3000;
		while(i < reachable_number)
		{
			xposition = atoi(x_position[i]);
			yposition = atoi(y_position[i]);

			//calculate distance
			distance_number = (destinationx - xposition)*(destinationx - xposition) + (destinationy - yposition) * (destinationy - yposition);

			if(distance_number < distance)
			{
				distance = distance_number;
				j = i;
			}
			i = i + 1;
		}
		
		char *ID1;
		ID1 = malloc(30 * sizeof (char ));
		strcpy(ID1,I_D[j]);

		//return next id
		return ID1;
	}

//Finding the next ID in received datamessage
char* range_recv(char I_D[30][30], char x_position[30][30], char y_position[30][30], int destinationx, int destinationy, int reachable_number, char datamessage_recv[30][30])
{
		int i = 0,j,k=0,j_linshi;
		int xposition,yposition,distance_number;
		int distance;

		char hoplist[30][30];

		int hoplist_length = atoi(datamessage_recv[4]);

		//copy the hoplist from datamessage
		while (k<hoplist_length)
		{
			strcpy(hoplist[k],datamessage_recv[5+k]);
			k++;
		}

		distance = 3000;
		while(i < reachable_number)
		{
			int count = 0;
			xposition = atoi(x_position[i]);
			yposition = atoi(y_position[i]);
			distance_number = (destinationx - xposition)*(destinationx - xposition) + (destinationy - yposition) * (destinationy - yposition);
			
			//Check if the ID we picked is the same as some one in the hoplist
			if(distance_number < distance)
			{		
				j_linshi = i;
				for(int u = 0; u < hoplist_length;u++)
				{
					if(strcmp(I_D[j_linshi],hoplist[u]) ==0 )
					{
						count = 1;
						break;
					}
				}
				if(count == 0 ){
					distance = distance_number;
					j = i;
				}				
			}
			i = i + 1;
		}
		char *ID1;
		ID1 = malloc(30 * sizeof (char ));
		strcpy(ID1,I_D[j]);

		//return next id
		return ID1;
}

//WHERE function
char** where(char *read3, int sock_num1)
{
	int n;
    char where[30][30];
    memset(where,0,sizeof(where));
    strcpy(where[0], "WHERE");

    //cut ID
    const char s[2] = " ";
   
	char ID[50];
	memset(ID, 0, sizeof(ID));
  	strcpy(ID, strtok(read3, s));
	strcpy(ID, strtok(NULL, s));
	
	strcpy(where[1], ID);

	//Send WHERE to the server
    send(sock_num1, where, strlen(where), 0);

    char where_recv[30][30];
    memset(where_recv, 0, sizeof(where_recv));
    //check THERE message
    while(1)
    {
    	if((n = recv(sock_num1, where_recv, sizeof(where_recv),0)) > 0)
    	{
    		if(strcmp(where_recv[0],"THERE") == 0)
    		{
    			break;
    		}
		}
	}
	for(int i = 0; i < 10; i++){
		printf("%s\n", where_recv[i]);
	}
	//convert double arrays to double pointers to return
	char **where_recv_return;
	where_recv_return = malloc(30 * sizeof (char *));
	int i;
	for(i = 0; i < 30; i++)
	{
		where_recv_return[i] = 	malloc(30 * sizeof (char));
	}

	for(i = 0; i < 30; i++)
	{
		strcpy(where_recv_return[i],where_recv[i]);
	} 
	return where_recv_return;
}

//MOVE function
char** move(char read[1024], int sock_num, char updateposition1[30][30])
{
	int n;
	
	char move[30][30];
	memset(move,0,sizeof(move));

    strcpy(move[0], "MOVE");

    char *position;
    position = malloc(30*sizeof(char));
    const char s[2] = " ";
  	strcpy(position, strtok(read, s));

	strcpy(position, strtok(NULL, s));
	for(int g = 0; g < strlen(position); g++){
	if(position[g] == '\n'){
	      	position[g] = '\0';
	    }
	}

	//Renew the updateposition
    strcpy(move[1],position);
	memset(updateposition1[3],0,30);
    strcpy(updateposition1[3],move[1]);  //update current position

    strcpy(position, strtok(NULL, s));
    for(int g = 0; g < strlen(position); g++){
	if(position[g] == '\n'){
	      	position[g] = '\0';
	    }
	}
    strcpy(move[2],position);
    memset(updateposition1[4],0,30);
    strcpy(updateposition1[4],move[2]);  //update current position

    //Send move to the server
    send(sock_num, updateposition1, strlen(updateposition1), 0);

    char move_recv[30][30];
    memset(move_recv,0,sizeof(move_recv));
    
    //check the REACHABLE message   
    while(1)
    {
    	if((n = recv(sock_num, move_recv, sizeof(move_recv),0)) > 0)
    	{
    		if(strcmp(move_recv[0],"REACHABLE") == 0)
    		{
    			break;
    		}
		}
	}

	//convert double arrays to double pointers to return
	char **updateposition_return;
	updateposition_return = malloc(30 * sizeof (char *));

	int i;

	for(i = 0; i < 30; i++)
	{
		updateposition_return[i] = 	malloc(30 * sizeof (char));
	}

	for(i = 0; i < 30; i++)
	{
		strcpy(updateposition_return[i],updateposition1[i]);
	}

	free(position);

	return updateposition_return;
}

//senddata function
void senddata(char *read1, int sock_num, char updateposition1[30][30])
{
	int n;
	send(sock_num, updateposition1, 3000, 0);
	
    char senddata_recv[30][30];
    memset(senddata_recv,0,sizeof(senddata_recv));
    //check the REACHABLE message
    while(1)
    {
    	if((n = recv(sock_num, senddata_recv, sizeof(senddata_recv),0)) > 0)
    	{
    		if(strcmp(senddata_recv[0],"REACHABLE") == 0)

    		{
    			break;
    		}
		}
	}

	char send_data[30][30];
	memset(send_data,0,sizeof(send_data));

    //cut destination ID
    const char s[2] = " ";
	char destination_ID[50];
	memset(destination_ID, 0, sizeof(destination_ID));
  	strcpy(destination_ID, strtok(read1, s));
	strcpy(destination_ID, strtok(NULL, s));

	strcpy(send_data[0],"SENDDATA");
	for(int g = 0; g < strlen(destination_ID); g++){
	    if(destination_ID[g] == '\n'){
	      	destination_ID[g] = '\0';
	    }
	}
	
	//Check if the message can be delivered or not
	if(strcmp(senddata_recv[1],"0") == 0){
		printf("%s: Message from %s to %s could not be delivered.\n",updateposition1[1], updateposition1[1],destination_ID);
	}
	else{	
	strcpy(send_data[1], destination_ID);
	
	
	
	char read2[30];
	strcpy(read2,"SENDDATA ");
	strcat(read2, destination_ID);
	
	//Call WHERE function to get the position of the destination 

	char **where_recv_senddata1;
	where_recv_senddata1 = malloc(30 * sizeof (char *));

	int i;
	for(i = 0; i < 30; i++)
	{
		where_recv_senddata1[i] = malloc(30 * sizeof (char ));
	}
	where_recv_senddata1 = where(read2,sock_num);
	
	//convert double arrays to double pointers to return
	char where_recv_senddata [30][30];
	memset(where_recv_senddata,0,sizeof(where_recv_senddata));

	for (i = 0;i < 30; i++)
	{
		strcpy(where_recv_senddata[i],where_recv_senddata1[i]);
	}

	for(i=0;i<30;i++)
	{
		free(where_recv_senddata1[i]);
	}
	free(where_recv_senddata1);

	int destination_x = atoi(where_recv_senddata[2]);
	int destination_y = atoi(where_recv_senddata[3]);
	
	int reachablenumber;
	reachablenumber = atoi(senddata_recv[1]);

	char ID[30][30];
	memset(ID,0,sizeof(ID));

	char xposition[30][30];
	memset(xposition,0,sizeof(xposition));

	char yposition[30][30];
	memset(yposition,0,sizeof(yposition));

	int j = 0;

	//Copy the ID to a new double array
	//Copy the Xposition to a new double array
	//Copy the Yposition to a new double array
	while (j < reachablenumber)
	{

		strcpy(ID[j],senddata_recv[3*j+2]);
		strcpy(xposition[j],senddata_recv[3*j+3]);
		strcpy(yposition[j],senddata_recv[3*j+4]);	
		j = j + 1;
	}

	char *next_id;
	//Get Next ID
	
	next_id = range(ID,xposition,yposition,destination_x,destination_y,reachablenumber) ;
	
	char senddata_message[30][30];
	memset(senddata_message,0,sizeof(senddata_message));
	
	//construct DATAMESSAGE
    strcpy(senddata_message[0],"DATAMESSAGE");
    strcpy(senddata_message[1],updateposition1[1]);
    strcpy(senddata_message[2],next_id);
    strcpy(senddata_message[3],send_data[1]);
    strcpy(senddata_message[4],"1");
    strcpy(senddata_message[5],updateposition1[1]);

    //Send the datamessage to the server
    
	send(sock_num,senddata_message , strlen (senddata_message), 0);
	
	printf("%s: Sent a new message bound for %s.\n",senddata_message[1],senddata_message[3]);

	free(next_id);	
	}
}



int main(int argc,char*argv[])
{
	//read port number	
	struct addrinfo hints;
	struct addrinfo *res,*now;
	int rc;

	//construct socket
	int sockfd,n;
	fd_set rset, allset;

	memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_family   = AF_UNSPEC;
    hints.ai_protocol=0;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	
	if(rc != 0)
    {
        printf("error:%s",gai_strerror(rc));
        exit(0);
    }
    
    //Connection
 	for (now = res; now != NULL; now = now->ai_next)
 	{
        sockfd = socket(now->ai_family, now->ai_socktype,
                            now->ai_protocol);
        if (sockfd == -1)
              continue;

        if ((rc = connect(sockfd, now->ai_addr, now->ai_addrlen)) != -1)
                   break;           /* Success */ 
                      
        // close(sockfd);       
    }

	//store sensor id
	char *sensor_id;
	sensor_id = malloc(10 * sizeof (char *));
	sensor_id = argv[3];

	//store sensor range
	char *sensor_range;
	sensor_range = malloc(10 * sizeof (char *));
	sensor_range = argv[4];

	//store sensor x position
	char *sensor_xposition;
	sensor_xposition = malloc(10 * sizeof (char *));
	sensor_xposition = argv[5];

	//store sensor y position
	char *sensor_yposition;
	sensor_yposition = malloc(10 * sizeof (char *));
	sensor_yposition = argv[6];

	//construct UPDATEPOSITION
	char updateposition[30][30];
    strcpy(updateposition[0],"UPDATEPOSITION");
    strcpy(updateposition[1],sensor_id);
    strcpy(updateposition[2],sensor_range);
    strcpy(updateposition[3],sensor_xposition);
    strcpy(updateposition[4],sensor_yposition);

    //send UPDATEPOSITION
	send(sockfd, updateposition, strlen(updateposition), 0);   
    char sensor_reachable[30][30];
    memset(sensor_reachable,0,sizeof(sensor_reachable));

    //check the REACHABLE message
    while(1)
    {
    	if((n = recv(sockfd, sensor_reachable, sizeof(sensor_reachable),0)) > 0)
    	{
    		if(strcmp(sensor_reachable[0],"REACHABLE") == 0)
    		{
    			break;
    		}
		}
	}


	//Construct Select to deal with STDIN and receive from the server
	char recv_datamessage[30][30];
	int fd[2];
	FD_ZERO(&rset);
	fd[0] = 0;
	fd[1] = sockfd;
	FD_SET(fd[0], &rset);
	FD_SET(fd[1], &rset);
	allset = rset;

    while(1)
    {   	
    	rset = allset;
    	select(sockfd+1, &rset, NULL, NULL, NULL);
		int y;

		//Select
		for(int w = 0; w < 2; w++)
		{
			//Check if the STDIN is active or server is active
			if(FD_ISSET(fd[w], &rset))
			{
				memset(recv_datamessage,0,sizeof(recv_datamessage));
			
			if((y = read(fd[w], recv_datamessage, sizeof(recv_datamessage)))>0)
			{
				// for(int o = 0; o < 10; o++){
    //               printf("after read %s\n", recv_datamessage[o]);
    //             }
				
				//handle MOVE instruction
				if(recv_datamessage[0][0] == 'M')
				{
					char **move_recv;				
					int g;

					move_recv = move(recv_datamessage[0],sockfd,updateposition);

					memset(updateposition[3],0,30);
					memset(updateposition[4],0,30);

					for(g = 0;g < 30;g++)
					{
						strcpy(updateposition[g],move_recv[g]);
					}

					for(g = 0; g < 30; g++)
					{
						free(move_recv[g]);
					}
					free(move_recv);
					
				}

				//handle senddata
				if(recv_datamessage[0][0] == 'S')
				{
					senddata(recv_datamessage[0],sockfd,updateposition);
				}

				//handle QUIT
				if(recv_datamessage[0][0] == 'Q')
				{					
					close(sockfd);
				}

				//handle WHERE
				if(recv_datamessage[0][0] == 'W')
				{
					where(recv_datamessage[0],sockfd);
				}

                //handle the message from the server
				if(strcmp(recv_datamessage[0],"DATAMESSAGE") == 0)

				{
					//check if the message can be received or not
					if(strcmp(recv_datamessage[3], sensor_id) == 0)
					{
						printf("%s: Message from %s to %s succesfully received.\n",sensor_id,recv_datamessage[1],recv_datamessage[3]);
					}

					//updateposition
					send(sockfd, updateposition, strlen(updateposition), 0);
					memset(sensor_reachable,0,sizeof(sensor_reachable));
					
					while(1)
					{
						if((n = recv(sockfd, sensor_reachable, sizeof(sensor_reachable),0)) > 0)
						{
							if(strcmp(sensor_reachable[0],"REACHABLE") == 0)
							{
								break;
							}
						}
					}
					
					
					if(strcmp(recv_datamessage[3], sensor_id) != 0)
					{

						//Check if the message can be delivered or not
						int o,k,count = 0;
						for(o = 0;o<30;o++)
						{
							for(k=0;k<30;k++)
							{
								if(strcmp(sensor_reachable[3*o+2],recv_datamessage[k+5]) == 0)
								{
									count = 1;
								}
							}
						}
						if(count == 0){
							printf("%s: Message from %s to %s could not be delivered.\n",sensor_id,recv_datamessage[1],recv_datamessage[3]);
							break;
						}
						
						//Renew the datamessage 
						char datamessage_where[30][30];
						memset(datamessage_where,0,sizeof(datamessage_where));

						//Get destination information
						strcpy(datamessage_where[0], "WHERE");
						strcpy(datamessage_where[1],recv_datamessage[3]);

						send(sockfd, datamessage_where, strlen(datamessage_where), 0);

						char datamessage_where_recv[30][30];
						memset(datamessage_where_recv,0,sizeof(datamessage_where_recv));						
						
						while(1)
						{
							if((n = recv(sockfd, datamessage_where_recv, sizeof(datamessage_where_recv),0)) > 0)
							{
								if(strcmp(datamessage_where_recv[0],"THERE") == 0)
								{
									break;
								}
							}
						}
					
						float destination_x = atof(datamessage_where_recv[2]);
						float destination_y = atof(datamessage_where_recv[3]);
						int u = 0;
						int reachablenumber;
						reachablenumber = atoi(sensor_reachable[1]);
						char ID[30][30];
						char xposition[30][30];
						char yposition[30][30];

						memset(ID,0,sizeof(ID));
						memset(xposition,0,sizeof(xposition));
						memset(yposition,0,sizeof(yposition));

						while (u < reachablenumber)
						{
							strcpy(ID[u],sensor_reachable[3*u+2]);
							strcpy(xposition[u],sensor_reachable[3*u+3]);
							strcpy(yposition[u],sensor_reachable[3*u+4]);
							u = u + 1;
						}

						char *next_id;

						//Call range_recv function and Get next ID
						next_id = range_recv(ID,xposition,yposition,destination_x,destination_y,reachablenumber,recv_datamessage);

						int hoplist_length;
						hoplist_length = atoi(recv_datamessage[4]);

						memset(recv_datamessage[2],0,30);
						strcpy(recv_datamessage[2],next_id);
						memset(recv_datamessage[5+hoplist_length],0,30);
						strcpy(recv_datamessage[5+hoplist_length],sensor_id);
						
						//renew hoplist length
						char hoplist_length_array[30];
						hoplist_length = hoplist_length + 1;
						sprintf(hoplist_length_array, "%d", hoplist_length);
					
						//renew hoplist
						memset(recv_datamessage[4],0,30);
						strcpy(recv_datamessage[4],hoplist_length_array);  //hoplist_length + 1
						
						//send new datamessage to server
						send(sockfd, recv_datamessage, strlen(recv_datamessage), 0);
						
						printf("%s: Message from %s to %s being forwarded through %s\n", sensor_id, recv_datamessage[1],recv_datamessage[3],sensor_id);
						free(next_id);
					
					}

				}
				
			} // if read
			} //if fdisset
			
		}//for

		memset(recv_datamessage,0,sizeof(recv_datamessage));

    }//while

}
