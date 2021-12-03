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

#define STDIN 0


//struct
struct cliendata
{
  unsigned int fd;
  char sensorid[50];
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

  ser_addr.sin_family = PF_INET;
  ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //zheng
  ser_addr.sin_port = htons(port);

  if (bind(listenfd, (const struct sockaddr *) &ser_addr,sizeof(ser_addr)) < 0){
      perror("Binding");
      return -1;
  }
  return listenfd;
}

//helper function to read file from base station file
char*** readfile(char* filename, char*** base_info)
{
  char buf[1024];
  int k = 0;
  //initialize container for base station file.
  base_info = malloc(50 * sizeof(char**));
  for(int i = 0; i < 50; i++)
  {
    base_info[i] = malloc(40* sizeof(char*));
    for(int j = 0; j < 40; j++)
    {
      base_info[i][j] = malloc(40 * sizeof(char));
    }
  }

	FILE *f = fopen(filename, "r");

	while( fgets(buf, 1024, f) != NULL)
	{
    int j = 0;
    int i = 0;
    int q = 0;
		while(buf[j] != '\n')
		{
      if(buf[j] == ' '){
        i++; //jump to another word
        q = 0; //clear q for the new word.
        j++;
        continue;
      }
      base_info[k][i][q] = buf[j];
      q++; //jump to another char
      j++;

		}
    //clear buffer for another row.
    memset(buf, 0, sizeof(buf));
    k++; //jump to another row

  }
  return base_info;
}





//this used to send msg
void sending( int sock, char* buffer, int len, struct sockaddr_in addr){
    int n = sendto(sock, buffer, len, 0, (const struct sockaddr *) &addr,sizeof (struct sockaddr_in));
    if(n==-1)
    {
      perror("Send Error");
    }
}

//helper function find and return index of sensor_buf
int sensor_isfound(char id[], char*** _buf, int count){

  if(count == 0){
    return -1;
  }
  for(int i = 0; i < count; i++){
    if(strcmp(_buf[i][1], id) == 0){
      return i;
    }
  }
  return -1;
}

//helper function to find and return index of base_buf
int base_isfound(char id[], char*** _buf, int count){

  if(count == 0){
    return -1;
  }
  for(int i = 0; i < count; i++){
    if(strcmp(_buf[i][0], id) == 0){
      return i;
    }
  }
  return -1;
}

//helper function to store information of sensor.
char*** pushback(char buffer[][40], char*** sensorbuf, int count){
  for(int i = 0; i < 5; i++){
    sensorbuf[count][i] = buffer[i];
  }
  return sensorbuf;
}

//helper function to record changed position of sensor.
char*** edit_sensor(char buffer[][40], char*** sensorbuf, int count){
  for(int i = 0; i < count; i++){
    if(strcmp(sensorbuf[i][1], buffer[1]) == 0){
      for(int j = 0; j < 5; j++){
        strcpy(sensorbuf[i][j], "");
        strcpy(sensorbuf[i][j], buffer[j]);
      }
    }
  }
  return sensorbuf;
}

//helper function to judge whether another point is in the range of this point.
int inrange(int thisx, int thisy, int radius, int thatx, int thaty){
  int e=0;
  int t = (thisx - thatx)*(thisx - thatx) + (thisy - thaty)*(thisy - thaty);
  if( t <= radius*radius){
     if( !((thisx == thatx) && (thisy == thaty))){
      e=1;
  }else{
    e=0;
  }}
  return e;
}

//helper function to find reachable ID.
char** sensor_find_reachable(char*** sensorbuf, char*** basebuf, char sensorid[], int sensorcount, int basecount){

  char** reachable;
  reachable = malloc(50 * sizeof(char*));
  int a;
  for(int i = 0; i < 50; i++)
  {
    reachable[i] = malloc(40* sizeof(char));
    memset(reachable[i], 0, 40);
  }

  a = sensor_isfound(sensorid, sensorbuf, sensorcount);

  int radius = atoi(sensorbuf[a][2]);
  int thisx = atoi(sensorbuf[a][3]);
  int thisy = atoi(sensorbuf[a][4]);
  int tmp_count = 2;



  //1. find reachable sensor
  for(int i = 0; i < sensorcount; i++){

    int thatx = atoi(sensorbuf[i][3]);
    int thaty = atoi(sensorbuf[i][4]);

      int e;
      e = inrange(thisx, thisy, radius, thatx, thaty);
    if(e){


      strcpy(reachable[tmp_count], sensorbuf[i][1]);
      strcpy(reachable[tmp_count+1], sensorbuf[i][3]);
      strcpy(reachable[tmp_count+2], sensorbuf[i][4]);
      tmp_count = tmp_count + 3;
    }
  }

  //2. find reachable base.
  for(int i = 0; i < basecount; i++){
    int thatx = atoi(basebuf[i][1]);
    int thaty = atoi(basebuf[i][2]);
    if(inrange(thisx, thisy, radius, thatx, thaty)){
      strcpy(reachable[tmp_count], basebuf[i][0]);
      strcpy(reachable[tmp_count+1], basebuf[i][1]);
      strcpy(reachable[tmp_count+2], basebuf[i][2]);
      tmp_count = tmp_count + 3;
    }
  }
  int numreachable = (tmp_count-2) / 3;
  sprintf(reachable[1],"%d", numreachable);
  strcpy(reachable[0], "REACHABLE");
  return reachable;
}

char** base_find_reachable(char*** sensor_buf, char*** base_buf, char baseid[], int sensor_count, int base_count){
  char** reachable;
  reachable = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    reachable[i] = malloc(40* sizeof(char));
  }
  int tmp_count = 2;
  int thisx = atoi(base_buf[base_isfound(baseid, base_buf, base_count)][1]);
  int thisy = atoi(base_buf[base_isfound(baseid, base_buf, base_count)][2]);
  //1. find reachable sensor
  for(int i = 0; i < sensor_count; i++){
    int thatx = atoi(sensor_buf[i][3]);
    int thaty = atoi(sensor_buf[i][4]);
    int thatradius = atoi(sensor_buf[i][2]);
    if(inrange(thisx, thisy, thatradius, thatx, thaty)){
      strcpy(reachable[tmp_count], sensor_buf[i][1]);
      strcpy(reachable[tmp_count+1], sensor_buf[i][3]);
      strcpy(reachable[tmp_count+2], sensor_buf[i][4]);
      tmp_count = tmp_count + 3;
    }
  }
  //2. find reachable base
  int loopnum = atoi(base_buf[base_isfound(baseid, base_buf, base_count)][3]) + 4;
  for(int i = 4; i < loopnum; i++){
    strcpy(reachable[tmp_count], base_buf[base_isfound(baseid, base_buf, base_count)][i]);
    strcpy(reachable[tmp_count+1], base_buf[base_isfound(reachable[tmp_count], base_buf, base_count)][1]);
    strcpy(reachable[tmp_count+2], base_buf[base_isfound(reachable[tmp_count], base_buf, base_count)][2]);
    tmp_count = tmp_count + 3;
  }
  int numreachable = (tmp_count-2) / 3;
  sprintf(reachable[1],"%d", numreachable);
  strcpy(reachable[0], "REACHABLE");
  return reachable;
}

//helper function to filter reachable with hoplist
char** filter_reachable(char** msg, char** reachable)
{
  char** new_reachable;
  new_reachable = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    new_reachable[i] = malloc(40* sizeof(char));
  }
  int j = 2;
  int k = 2;
  if(strcmp(msg[4], "0") == 0){
    for(int i = 0; i < 50; i++)
    {
      strcpy(new_reachable[i], reachable[i]);
    }
  }
  else
  {

    while(j < 2 + atoi(reachable[1])*3)
    {
      bool found = false;
      for(int i = 5; i < 5 + atoi(msg[4]); i++)
      {
        if(strcmp(reachable[j], msg[i]) == 0)
        {
          found = true;
        }
      }
      if(found == false){
        strcpy(new_reachable[k], reachable[j]);
        strcpy(new_reachable[k+1], reachable[j+1]);
        strcpy(new_reachable[k+2], reachable[j+2]);
        k = k + 3;
      }
      j = j + 3;
    }
    sprintf(new_reachable[1],"%d", (k-2)/3 );
  }
  return new_reachable;
}

//helper function to find next id.
char* find_nextid(char* thisx, char* thisy, char** reachable){
  int small_distance = -1;
  int tmp = 3;
  char* nextid = malloc(50* sizeof(char));
  int numreachable = atoi(reachable[1]);
  int distance;
  // strcpy(nextid, reachable[2]);
  while(tmp < 2 + numreachable*3){
    int thatx = atoi(reachable[tmp]);
    int thaty = atoi(reachable[tmp+1]);
    int this_x = atoi(thisx);
    int this_y = atoi(thisy);
    distance = (this_x - thatx)*(this_x - thatx) + (this_y - thaty)*(this_y - thaty);
    if(distance == small_distance){
      if(nextid[13] > reachable[tmp-1][13]){
        strcpy(nextid, reachable[tmp-1]);
      }
    }
    if(distance < small_distance || small_distance == -1){
      small_distance = distance;
      strcpy(nextid, reachable[tmp-1]);
    }

    tmp = tmp + 3;
  }
  return nextid;
}

//generate new message
char** new_msg(char origin_id[], char dest_id[], char*** base_buf, char*** sensor_buf, int base_count, int sensor_count)
{
  char** reachable;
  reachable = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    reachable[i] = malloc(40* sizeof(char));
  }
  char** datamsg;
  datamsg = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    datamsg[i] = malloc(40* sizeof(char));
  }
  int row_index = 2;
  int row;
  char* thisx = malloc(sizeof(char)*20);
  char* thisy = malloc(sizeof(char)*20);
  datamsg[0] = "DATAMESSAGE";
  datamsg[1] = origin_id;
  datamsg[3] = dest_id;
  sprintf(datamsg[4], "%d", 1);
  datamsg[5] = origin_id;
  //before we begin, make sure origin id is not a sensor
  if(sensor_isfound(origin_id, sensor_buf, sensor_count) != -1) exit(1);

  //1. if origin id is control
  if(strcmp(origin_id, "CONTROL") == 0)
  {
    //before we find the next, we need to expell the sensor id from reachable list
    for(int i = 0; i < base_count; i++)
    {
      reachable[row_index] = base_buf[i][0];
      reachable[row_index+1] = base_buf[i][1];
      reachable[row_index+2] = base_buf[i][2];
      row_index += 3;
    }

    //case the dest id is a sensor
    if((row = sensor_isfound(dest_id, sensor_buf, sensor_count)) != -1)
    {
      //find position of dest id
      thisx = sensor_buf[row][3];
      thisy = sensor_buf[row][4];

      //find next

      datamsg[2] = find_nextid(thisx, thisy, reachable);
    }

    //case the dest id is base
    if((row = base_isfound(dest_id, base_buf, base_count)) != -1)
    {
      //find position of dest id
      thisx = base_buf[row][1];
      thisy = base_buf[row][2];
      //find next
      datamsg[2] = find_nextid(thisx, thisy, reachable);
    }
  }

  //2. if origin is base station
  if(strcmp(origin_id, "CONTROL") != 0)
  {
    //find reachable
    reachable = base_find_reachable(sensor_buf, base_buf, origin_id, sensor_count, base_count);
    //case the dest id is a sensor
    if((row = sensor_isfound(dest_id, sensor_buf, sensor_count)) != -1)
    {
      //find position of dest id
      thisx = sensor_buf[row][3];
      thisy = sensor_buf[row][4];

      //find next
      datamsg[2] = find_nextid(thisx, thisy, reachable);
    }

    //case the dest id is base
    if((row = base_isfound(dest_id, base_buf, base_count)) != -1)
    {
      //find position of dest id
      thisx = base_buf[row][1];
      thisy = base_buf[row][2];
      //find next
      datamsg[2] = find_nextid(thisx, thisy, reachable);
    }
  }
  for(int i = 0; i < 50; i++){
    free(reachable[i]);
  }
  free(reachable);
  free(thisx);
  free(thisy);
  return datamsg;
}






//helper function to store anything from stdin.
char** store_stdin(char linebuffer[]){ //?
  char** output;
  output = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    output[i] = malloc(40* sizeof(char));
  }
  int q = 0;
  int j = 0;
  //as we know the structure of SENDATA, we begin loop from origin ID.
  for(int i = 8; i < strlen(linebuffer); i++){
    if(linebuffer[i] == '\n'){
      break;
    }
    if(linebuffer[i] != ' '){
      output[q][j] = linebuffer[i];
      j++;
    }else{
      q++;
      j = 0;
    }
  }
  return output;
}

//function to deliver message from base to base or from server to base.
char** deli_to_base(char** datamsg, char*** base_buf, char*** sensor_buf, int base_count, int sensor_count)
{

  char thisid[50];
  char nextid[50];
  char** new_reachable;
  char** ori_reachable;
  char** reach;
  int row;

  char** new_msg;
  new_msg = malloc(50 * sizeof(char*));
  for(int i = 0; i < 50; i++)
  {
    new_msg[i] = malloc(40* sizeof(char));
  }
  char* thisx = malloc(sizeof(char)*20);
  char* thisy = malloc(sizeof(char)*20);
  strcpy(thisid, datamsg[2]);
  if(base_isfound(thisid, base_buf, base_count) != -1){
    reach = base_find_reachable(sensor_buf, base_buf, thisid, sensor_count, base_count);

    ori_reachable = filter_reachable(datamsg, base_find_reachable(sensor_buf, base_buf, thisid, sensor_count, base_count));

  }
  if(sensor_isfound(thisid, sensor_buf, sensor_count) != -1){


    reach = sensor_find_reachable(sensor_buf, base_buf, thisid, sensor_count, base_count);

    ori_reachable = filter_reachable(datamsg, reach);
  }
  memset(thisid, 0, sizeof(thisid));




  //first case, message matches
  if(strcmp(datamsg[2], datamsg[3]) == 0){
    printf("%s: Message from %s to %s successfully received.\n", datamsg[2], datamsg[1], datamsg[3]);
    fflush(stdout);
    return datamsg;
  }
  //second case to discard message
  else if(strcmp(ori_reachable[1], "0") == 0 && strcmp(datamsg[2], datamsg[3]) != 0)
  {
    datamsg[2][14] = '\0';

    printf("%s: Message from %s to %s being forwarded through %s\n", datamsg[2], datamsg[1], datamsg[3], datamsg[2]);
    fflush(stdout);
    printf("%s: Message from %s to %s could not be delivered.\n", datamsg[2], datamsg[1], datamsg[3]);
    fflush(stdout);
    return datamsg;
  }
  else
  {
    datamsg[2][14] = '\0';
    printf("%s: Message from %s to %s being forwarded through %s\n", datamsg[2], datamsg[1], datamsg[3], datamsg[2]);
    fflush(stdout);
  }
  strcpy(thisid, datamsg[3]);
  //third case:find thisx and thisy
  if((row = base_isfound(thisid, base_buf, base_count)) != -1){
    strcpy(thisx, base_buf[row][1]);
    strcpy(thisy, base_buf[row][2]);
  }
  if((row = sensor_isfound(thisid, sensor_buf, sensor_count)) != -1)
  {
    strcpy(thisx, sensor_buf[row][3]);
    strcpy(thisy, sensor_buf[row][4]);
  }

  memset(thisid, 0, sizeof(thisid));


  //here we generate our new msg.
  for(int i = 0; i < 50; i++)
  {
    strcpy(new_msg[i], datamsg[i]);
  }

  reach = base_find_reachable(sensor_buf, base_buf, datamsg[2], sensor_count, base_count);

  //before use next id function, find the reachable list of next id.
  new_reachable = filter_reachable(datamsg, base_find_reachable(sensor_buf, base_buf, datamsg[2], sensor_count, base_count));



  new_msg[2] = find_nextid(thisx, thisy, new_reachable);
  strcpy(nextid, new_msg[2]);
  strcpy(new_msg[atoi(datamsg[4]) + 5], datamsg[2]);
  sprintf(new_msg[4], "%d", atoi(datamsg[4]) + 1);


  for(int i = 0; i < 50; i++)
  {
    strcpy(datamsg[i], new_msg[i]);
    free(new_msg[i]);
  }
  free(new_msg);
  if(base_isfound(nextid, base_buf, base_count) != -1)
  {

    deli_to_base(datamsg, base_buf, sensor_buf, base_count, sensor_count);
  }
  else
  {

      for(int i = 0; i < 50; i++){
        free(new_reachable[i]);
        free(ori_reachable[i]);
      }
      free(new_reachable);
      free(ori_reachable);
      free(thisx);
      free(thisy);
      
  }
  return datamsg;

}


//function to clear the server
void clear_server(char*** base_buf, char*** sensor_buf, struct cliendata client[], fd_set allset)
{
  for(int i = 0; i < 50; i++)
  {
    for(int j = 0; j < 40; j++)
    {
      free(base_buf[i][j]);
    }
    free(base_buf[i]);
  }
  free(base_buf);
  for(int i = 0; i < 50; i++)
  {
    for(int j = 0; j < 40; j++)
    {
      free(sensor_buf[i][j]);
    }
    free(sensor_buf[i]);
  }
  free(sensor_buf);
  for(int i = 0; i < 50; i++)
  {
    if(client[i].fd != -1){
      strcpy(client[i].sensorid, "");
      close(client[i].fd);
      FD_CLR(client[i].fd, &allset);
      client[i].fd = -1;
    }
  }
}




int main(int argc, char* argv[])
{
  setvbuf(stdout, NULL, _IONBF, 0);
	struct sockaddr_in	cliaddr;
	socklen_t			clilen;

  fd_set rset, allset;
  int maxfd, connfd, nready, n;
  struct cliendata client[50];

  int serverfd;


  char*** base_buf;
  base_buf = malloc(50 * sizeof(char**));
  for(int i = 0; i < 50; i++)
  {
    base_buf[i] = malloc(50* sizeof(char*));
    for(int j = 0; j < 50; j++)
    {
      base_buf[i][j] = malloc(40 * sizeof(char));
    }
  }

  char*** sensor_buf;
  sensor_buf = malloc(50 * sizeof(char**));
  for(int i = 0; i < 50; i++)
  {
    sensor_buf[i] = malloc(50* sizeof(char*));
    for(int j = 0; j < 50; j++)
    {
      sensor_buf[i][j] = malloc(40 * sizeof(char));
    }
  }


  char linebuffer[1024]; //buffer read from command line.
  memset(linebuffer, 0, sizeof(linebuffer));
  char buffer[50][40];
  memset(buffer, 0, sizeof(buffer)); //make sure the buffer is cleared.

  char stdin_buf[50][40];
  memset(stdin_buf, 0, sizeof(stdin_buf));

  char reachable[50][40];
  memset(reachable, 0, sizeof(reachable));
  char there[50][40];
  memset(there, 0, sizeof(there));
  char msg[50][40];
  memset(msg, 0, sizeof(msg));

  //count used to record number of row of base and sensor.
  int base_count = 0;
  int sensor_count = 0;
  int row;
	int serv_port = atoi(argv[1]);
	serverfd = create_addr(serv_port);
  listen(serverfd, 1024);
  maxfd = serverfd;
  for(int i = 0;i < 50;i++)
  {
    client[i].fd = -1;
    strcpy(client[i].sensorid, "");
  }
  FD_ZERO(&allset);
  FD_SET(serverfd, &allset);
  client[0].fd = STDIN;
  FD_SET(client[0].fd, &allset);

	//read base station file and store it into base_buf.
  base_buf = readfile(argv[2], base_buf);

  while(base_buf[base_count][0][0] == 'b'){
    base_count++;
  }



	while(1)
  {



        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        //accept the new connection
        if(FD_ISSET(serverfd, &rset))
        {
          clilen = sizeof(cliaddr);
          connfd = accept(serverfd, (struct sockaddr *) &cliaddr,& clilen);
          for(int i = 0; i < 50; i++)
          {
            if(client[i].fd == -1)
            {
              client[i].fd = connfd;
              break;
            }
          }
          FD_SET(connfd, &allset);
          if (connfd > maxfd)
            maxfd = connfd;

          if (--nready <= 0)
            continue;

        }

        for (int i = 0; i < 50; i++)
        {
          int clifd;
          if ( (clifd = client[i].fd) == -1)
            continue;
          if (FD_ISSET(clifd, &rset))
          {

            if((n = read(client[i].fd, buffer, sizeof(buffer))) > 0)
            {



              if(buffer[0][0] == 'Q')
              {
                clear_server(base_buf, sensor_buf, client, allset);
              }

              if(strcmp(buffer[0], "UPDATEPOSITION") == 0)
              {


                strcpy(client[i].sensorid, buffer[1]);
                if(sensor_isfound(buffer[1], sensor_buf, sensor_count) == -1){
                  for(int k = 0; k < 50; k++){
                    strcpy(sensor_buf[sensor_count][k], buffer[k]);
                  }
                  // sensor_buf1 = sensor_buf;
                  sensor_count++;
                }

                else{
                  sensor_buf = edit_sensor(buffer, sensor_buf, sensor_count);

                }
                char** reachable2;
                reachable2 = sensor_find_reachable(sensor_buf, base_buf, buffer[1], sensor_count, base_count);
                for(int j = 0; j < 50; j++){
                  strcpy(reachable[j], reachable2[j]);
                }
                int len;
                len = 40 * (atoi(reachable2[1])*3 +2);
                send(client[i].fd, reachable, len, 0);
                memset(reachable, 0, sizeof(reachable));
                memset(buffer, 0, sizeof(buffer));
                for(int j = 0; j < 50; j++){
                  free(reachable2[j]);
                }
                free(reachable2);
              }
              //deal with where request
              if(strcmp(buffer[0], "WHERE") == 0)
              {
                //when the destination is belong to sensor
                strcpy(there[1], buffer[1]);
                strcpy(there[0], "THERE");
                if((row = sensor_isfound(buffer[1], sensor_buf, sensor_count)) != -1){
                  strcpy(there[2], sensor_buf[row][3]);
                  strcpy(there[3], sensor_buf[row][4]);
                  send( client[i].fd, there, 160, 0);
                }
                //when the destination is belong to base
                if((row = base_isfound(buffer[1], base_buf, base_count)) != -1){
                  strcpy(there[2], base_buf[row][1]);
                  strcpy(there[3], base_buf[row][2]);
                  send( client[i].fd, there, 160, 0);
                }
                memset(there, 0, sizeof(there));
                memset(buffer, 0, sizeof(buffer));

              }

              //deal with datamessage
              if(strcmp(buffer[0], "DATAMESSAGE") == 0)
              {
                if((row = base_isfound(buffer[2], base_buf, base_count)) != -1)
                {
                  char** msg_to_deli;
                  msg_to_deli = malloc(50 * sizeof(char*));
                  for(int k = 0; k < 50; k++)
                  {
                    msg_to_deli[k] = malloc(40* sizeof(char));
                  }
                  for(int j = 0; j < 50; j++){
                    strcpy(msg_to_deli[j], buffer[j]);
                  }


                  msg_to_deli = deli_to_base(msg_to_deli, base_buf, sensor_buf, base_count, sensor_count);
                  for(int j = 0; j < 50; j++){
                    strcpy(msg[j], msg_to_deli[j]);
                  }
                  int len;
                  len = 40*(atoi(msg_to_deli[4]) + 5);
                  for(int j = 0; j < 50; j++){
                    if(strcmp(client[j].sensorid, msg[2]) == 0){

                      send(client[j].fd, msg, len, 0);
                    }
                  }

                  for(int k = 0; k < 50; k++)
                  {
                    memset(msg_to_deli[k], 0, 40);
                    free(msg_to_deli[k]);
                  }
                  free(msg_to_deli);
                }
                else
                {
                  int len;
                  len = 40*(atoi(buffer[4]) + 5);
                  for(int j = 0; j < 50; j++){
                    if(strcmp(client[j].sensorid, buffer[2]) == 0)
                    {

                      send(client[j].fd, buffer, len, 0);
                    }
                  }
                }
                memset(buffer, 0, sizeof(buffer));
              }
            }
          }
        }
  }
  return 0;
}
