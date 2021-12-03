#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>



#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>



        int main(int argc, char **argv)
        {
            int result;
            struct addrinfo curhints,hints;
            struct addrinfo *now,*res;
            struct sockaddr_in *addr;
            
         
            memset(&hints,0,sizeof(hints));
                
            hints.ai_family=AF_UNSPEC;
            hints.ai_socktype=SOCK_STREAM;
            hints.ai_protocol=0;
            hints.ai_flags=AI_PASSIVE;

            result = getaddrinfo( argv[1], 0, &hints, &res );

            if(result != 0)
            {
                
                printf("error:%s",gai_strerror(result));
                exit(0);
            }

            for(now = res; now != NULL; now = now->ai_next){
                addr = (struct sockaddr_in*)now->ai_addr;

                char buf1[512];
                memset(buf1, 0, 512);
                if(now->ai_family == AF_INET)
                    printf("%s\n",inet_ntop(AF_INET, &addr->sin_addr, buf1, 128));

                char buf2[512];
                memset(buf2, 0, 512);
                if(now->ai_family == AF_INET6)
                    printf("%s\n",inet_ntop(AF_INET6, &addr->sin_addr, buf2, 128));
                
                

                
               // const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
            }
            freeaddrinfo(res);
        }