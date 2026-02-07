#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "protocol.h"

int main(){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd<0){
        perror("socket");
        return 1;   
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr);

    if(connect(fd,(struct sockaddr*)&addr,sizeof(addr))<0){
        perror("connect");
        return 1;
    }
    const char *msg = "Hello, Framed World!";
    send_message(fd,msg,strlen(msg));
    char buffer[MAX_MSG+1];
    uint32_t len;
    if(recv_message(fd,buffer,&len)==0){
        printf("Echoed: %s\n",buffer);    
    }
    close(fd);
    return 0;
}