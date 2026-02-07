#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "protocol.h"

int main(){
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd < 0){
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if(bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))<0){
        perror("bind");
        return 1;
    }

    if(listen(server_fd,5)<0){
        perror("listen");
        return 1;
    }

    printf("Server is listening on port 8080...\n");

    int client_fd = accept(server_fd,NULL,NULL);
    if(client_fd <0){
        perror("accept");
        return 1;
    }

    char buffer[MAX_MSG+1];
    uint32_t len;
    int n = recv_message(client_fd,buffer,&len);
    if(recv_message(client_fd,buffer,&len)==0){
        printf("Received: %s\n",buffer);
        send_message(client_fd,buffer,len);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}