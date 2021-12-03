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

    // struct timeval
    // {
    //         time_t tv_sec;
            
    // };

char* range(char I_D[100][200], char x_position[100][200], char y_position[100][200], int destinationx, int destinationy, int reachable_number)
	{
		int i = 0,j;
		int xposition,yposition,distance_number;
		int distance;
		
		distance = 500;
		while(i < reachable_number)
		{
			xposition = atof(x_position[i]);
			yposition = atof(y_position[i]);
			distance_number = (destinationx - xposition)*(destinationx - xposition) + (destinationy - yposition) * (destinationy - yposition);
			//printf("啦啦啦%d,%d",distance_number,distance[0]);
			if(distance_number < distance)
			{
				distance = distance_number;
				j = i;
			}
			i = i + 1;
		}
		printf("ID%s\n", I_D[j]);
		char *ID1;
		ID1 = malloc(100 * sizeof (char ));
		strcpy(ID1,I_D[j]);

		return ID1;		
	}
char** where(char *read3, int sock_num1)
{
	int n;
    char where[100][200];
    memset(where,0,sizeof(where));

    strcpy(where[0], "WHERE");
    //cut ID
    const char s[2] = " ";
	char ID[50];  
	memset(ID, 0, sizeof(ID)); 
  	strcpy(ID, strtok(read3, s));
	strcpy(ID, strtok(NULL, s));
	printf("ID%c\n",ID[50]);
	strcpy(where[1], ID);
    send(sock_num1, where, sizeof(where), 0);

    char where_recv[100][200];
    //where_recv = malloc(200 * sizeof (char *));
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
	for(int y=0;y<7;y++){
		printf("问问%s\n",where_recv[y]);
	}
	
	char **where_recv_return;
	where_recv_return = malloc(100 * sizeof (char *));
	int i;
	for(i = 0; i < 100; i++)
	{
		where_recv_return[i] = 	malloc(200 * sizeof (char));
	}

	for(i = 0; i < 100; i++)
	{
		strcpy(where_recv_return[i],where_recv[i]);
	}
	 printf("1.83\n");
	return where_recv_return;
}

char** move(char read[1024], int sock_num, char updateposition1[100][200])
{
	int n;
	// char **move;
 //    move = malloc(3 * sizeof (char *));
	char move[100][200];
	memset(move,0,sizeof(move));
	
    // int new_xposition = atoi(argv2);
    // int new_yposition = atoi(argv3);
    strcpy(move[0], "MOVE");

    char *position;
    position = malloc(100*sizeof(char));
    const char s[2] = " ";
  	strcpy(position, strtok(read, s));

	strcpy(position, strtok(NULL, s));
	for(int g = 0; g < strlen(position); g++){
	if(position[g] == '\n'){
	      	position[g] = '\0';
	    }
	}
    strcpy(move[1],position);
	memset(updateposition1[3],0,200);
    strcpy(updateposition1[3],move[1]);  //update current position

    strcpy(position, strtok(NULL, s));
    for(int g = 0; g < strlen(position); g++){
	if(position[g] == '\n'){
	      	position[g] = '\0';
	    }
	}
    strcpy(move[2],position);
    memset(updateposition1[4],0,200);
    strcpy(updateposition1[4],move[2]);  //update current position

    		
    
    send(sock_num, updateposition1, 20000, 0); 

    char move_recv[100][200];
    memset(move_recv,0,sizeof(move_recv));
    //check the REACHABLE message
    printf("before recv\n");
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
	for(int i = 0; i < 20; i++){
		printf("after move reachable %s\n", move_recv[i]);
	}
	
	char **updateposition_return;
	updateposition_return = malloc(100 * sizeof (char *));

	int i;

	for(i = 0; i < 100; i++)
	{
		//where_recv[i] = where_recv_return[i] ;
		updateposition_return[i] = 	malloc(200 * sizeof (char));
	}

	for(i = 0; i < 100; i++)
	{
		strcpy(updateposition_return[i],updateposition1[i]);		  
	}

	free(position);

	return updateposition_return;
}

//senddata function
void senddata(char *read1, int sock_num, char updateposition1[100][200])
{
	int n;
	send(sock_num, updateposition1, 20000, 0);
	for(int i = 0; i < 10; i++){
		printf("senddata position %s\n",updateposition1[i]);
	}
    char senddata_recv[100][200];
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
	
	printf("fanhui1\n");
	char send_data[100][200];
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
	strcpy(send_data[1], destination_ID);
	printf("fanhui1.5\n");
	char **where_recv_senddata1;
	where_recv_senddata1 = malloc(100 * sizeof (char *));

	int i;
	for(i = 0; i < 100; i++)
	{
		where_recv_senddata1[i] = 	malloc(200 * sizeof (char ));
	}
	printf("fanhui1.8\n");
	char read2[100];
	strcpy(read2,"SENDDATA ");
	strcat(read2, destination_ID);
	where_recv_senddata1 = where(read2,sock_num);
	printf("fanhui2\n");
	char where_recv_senddata [100][200];
	memset(where_recv_senddata,0,sizeof(where_recv_senddata));

	for (i = 0;i < 100; i++)
	{
		strcpy(where_recv_senddata[i],where_recv_senddata1[i]);
	}

	for(i=0;i<100;i++)
	{
		free(where_recv_senddata1[i]);
	}
	free(where_recv_senddata1);
	//There [NODEID][Xposition][Yposition]
	int destination_x = atof(where_recv_senddata[2]);
	int destination_y = atof(where_recv_senddata[3]);

	int reachablenumber; 
	reachablenumber = atoi(senddata_recv[1]);

	char ID[100][200];
	memset(ID,0,sizeof(ID));

	char xposition[100][200];
	memset(xposition,0,sizeof(xposition));

	char yposition[100][200];
	memset(yposition,0,sizeof(yposition));


	for(int r = 0;r<20;r++){
		printf("updateposition %s\n",senddata_recv[r]);
	}
	int j = 0;
	while (j < reachablenumber)
	{
		
		strcpy(ID[j],senddata_recv[3*j+2]); 
		strcpy(xposition[j],senddata_recv[3*j+3]); 
		strcpy(yposition[j],senddata_recv[3*j+4]); 		
		j = j + 1;
	
	}

	char *next_id;

	next_id = range(ID,xposition,yposition,destination_x,destination_y,reachablenumber) ;
	printf("nest id%s\n",next_id);

	char senddata_message[100][200];
	memset(senddata_message,0,sizeof(senddata_message));
	//construct DATAMESSAGE
	printf("fanhui3.1\n");
    strcpy(senddata_message[0],"DATAMESSAGE");
    strcpy(senddata_message[1],updateposition1[1]);
    strcpy(senddata_message[2],next_id);
    strcpy(senddata_message[3],send_data[1]);
    strcpy(senddata_message[4],"0");
    printf("send data %s\n", send_data[1]);
    
    printf("fanhui3.2\n");
	send(sock_num,senddata_message , sizeof (senddata_message), 0);

	printf("%s: Sent a new message bound for %s.",senddata_message[1],senddata_message[3]);
	
	free(next_id);
	printf("fanhui3.3\n");

}



    	

int main(int argc,char*argv[])
{
	//read port number
	printf("1\n");
	struct addrinfo hints,*res,*now;
	int rc;

	//construct socket
	//建立socket
	int sockfd,n;
	
	memset(&hints, 0, sizeof(hints));
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_family   = AF_INET;
    hints.ai_protocol=0;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	printf("2\n");
	if(rc != 0)
    {                
        printf("error:%s",gai_strerror(rc));
        exit(0);
    }
    printf("3\n");
 	for (now = res; now != NULL; now = now->ai_next) 
 	{
        sockfd = socket(now->ai_family, now->ai_socktype,
                            now->ai_protocol);
        if (sockfd == -1)
              continue;

        if ((rc = connect(sockfd, now->ai_addr, now->ai_addrlen)) != -1)
                   break;                  /* Success */

         printf("4\n");
    }

    

   
	// struct sockaddr_in client;
	// struct sockaddr_in serveraddr;

	// bzero(&serveraddr, sizeof(serveraddr)); 
 	
 // 	serveraddr.sin_family = AF_INET;
 // 	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
 //    serveraddr.sin_port = htons(control_port);
    
 //    //check connection
 //    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) != 0) { 
 //        printf("connection with the server failed...\n"); 
 //        exit(0); 
 //    }
	
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

	//刚建立发updateposition
	// char **updateposition;
 //    updateposition = malloc(20 * sizeof (char *));
	char updateposition[100][200];
    strcpy(updateposition[0],"UPDATEPOSITION");
    strcpy(updateposition[1],sensor_id);
    strcpy(updateposition[2],sensor_range);
    strcpy(updateposition[3],sensor_xposition);
    strcpy(updateposition[4],sensor_yposition);
  
    printf("5\n");
	send(sockfd, updateposition, sizeof(updateposition), 0); //send
    printf("6\n");
    char sensor_reachable[100][200];
    memset(sensor_reachable,0,sizeof(sensor_reachable));
    //check the REACHABLE message
    while(1)
    {
    	// char **sensor_reachable;
    	// sensor_reachable = malloc(200 * sizeof (char *));
    	if((n = recv(sockfd, sensor_reachable, sizeof(sensor_reachable),0)) > 0)
    	{	    	
    		if(strcmp(sensor_reachable[0],"REACHABLE") == 0)    			
    		{    			 
    			break;
    		}	  
		}
	}
	for(int i = 0; i < 20; i++){
		printf("%s\n", sensor_reachable[i]);
	}

	printf("7\n");

    while(1)
    {
    	char read_line[1024];
		memset(read_line,0,1024);

		int flags = fcntl(STDIN_FILENO, F_GETFL);
		flags |= O_NONBLOCK;

     	if(fcntl(STDIN_FILENO, F_SETFL, flags) == -1){
        	exit(1);
        };

		
		int y;
		if((y = read(STDIN_FILENO, read_line, 1024))>0)
		{

		//handle move
    	if(read_line[0] == 'M')
    	{	
    		char **move_recv;
			// move_recv = malloc(100 * sizeof (char *));

			int g;
			// for(g = 0; g < 100; g++)
			// {
			// 	move_recv[g] = 	malloc(200 * sizeof (char));
			// }

			move_recv = move(read_line,sockfd,updateposition);

    		memset(updateposition[3],0,200);
    		memset(updateposition[4],0,200);

    		for(g = 0;g < 100;g++)
    		{
    			strcpy(updateposition[g],move_recv[g]);
    		}

    		for(g = 0; g < 100; g++)
			{
				free(move_recv[g]);
			}
    		free(move_recv);
    		printf("paohuilai\n");
    	}

    	//handle senddata
    	if(read_line[0] == 'S')
    	{
    		printf("8\n");
    		senddata(read_line,sockfd,updateposition);
    	}

    	//handle QUIT
    	if(read_line[0] == 'Q')
    	{
    		close(sockfd);
    	}

    	//handle WHERE
    	if(read_line[0] == 'W')
    	{
    		where(read_line,sockfd);
    	}

    	char recv_datamessage[100][200];
    	memset(recv_datamessage,0,sizeof(recv_datamessage));

    		// int flags = fcntl(sockfd, F_GETFL, 0);
      //     	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    	printf("datamessage %s\n",recv_datamessage[0]);
        struct timeval tv_out;
          	
    	tv_out.tv_sec = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
    	
    	printf("datamessage1 %s\n",recv_datamessage[0]);
    	int e=0;
    	e = recv(sockfd, recv_datamessage, sizeof(recv_datamessage),0);
		printf("datamessage2 %s\n",recv_datamessage[0]);
		printf("datamessage2 %s\n",recv_datamessage[1]);
	

    	if(strcmp(recv_datamessage[0],"DATAMESSAGE") == 0)
    		
    	{
    		if(strcmp(recv_datamessage[3], sensor_id) == 0)
    		{
    			printf("%s: Message from %s to %s succesfully received.\n",sensor_id,recv_datamessage[1],recv_datamessage[3]);
    		}
    		
    		//updateposition
    		send(sockfd, updateposition, sizeof(updateposition), 0);
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
    			int o,k,count = 0;
    			for(o = 0;o<100;o++){
    				for(k=0;k<100;k++){
    					if(strcmp(sensor_reachable[3*o+2],recv_datamessage[k+5]) == 0){
    						count = 1;
    					}
    				}
    			}
    			if(count == 0){
    				printf("%s: Message from %s to %s could not be delivered.",sensor_id,recv_datamessage[1],recv_datamessage[3]);
    			}
    		}
    		else
    		{
    			char datamessage_where[100][200];
    			memset(datamessage_where,0,sizeof(datamessage_where));

    			strcpy(datamessage_where[0], "Where");
    			strcpy(datamessage_where[1],recv_datamessage[3]);

    			send(sockfd, datamessage_where, sizeof(datamessage_where), 0);

    			char datamessage_where_recv[100][200];
    			memset(datamessage_where_recv,0,sizeof(datamessage_where_recv));
    		
    			while(1)
    			{
    				if((n = recv(sockfd, datamessage_where_recv, sizeof(datamessage_where_recv),0)) > 0)
    				{	    	
    					if(strcmp(datamessage_where_recv[0],"There") == 0)
    					{
    						break;
    					}	  
					}    
				}

				float destination_x = atof(datamessage_where_recv[2]);
				float destination_y = atof(datamessage_where_recv[3]);
				int u = 0;
				float reachablenumber;
				reachablenumber = atof(sensor_reachable[1]);
				char ID[100][200];
				char xposition[100][200];
				char yposition[100][200];
				
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

				next_id = range(ID,xposition,yposition,destination_x,destination_y,reachablenumber);

				int hoplist_length;
				hoplist_length = atoi(recv_datamessage[4]);
				

				char hoplist_length_array[20];
				sprintf(hoplist_length_array, "%d", hoplist_length);
				
				memset(recv_datamessage[2],0,200);
				strcpy(recv_datamessage[2],next_id);
				
				memset(recv_datamessage[5+hoplist_length],0,200);
				strcpy(recv_datamessage[5+hoplist_length],sensor_id);	
				
				hoplist_length = hoplist_length + 1;
				memset(recv_datamessage[4],0,200);
				strcpy(recv_datamessage[4],hoplist_length_array);  //hoplist_length + 1				


				send(sockfd, recv_datamessage, sizeof(recv_datamessage), 0);
				free(next_id);
    			printf("%s: Message from %s to %s being forwarded through %s", sensor_id, recv_datamessage[1],recv_datamessage[3],recv_datamessage[2]);
    		}

    	}
    }

    }


}


