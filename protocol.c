#include"protocol.h"
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int send_message(int fd,const char *data,uint32_t len){
    if(len>MAX_MSG){
        return -1;
    }
    uint32_t net_len = htonl(len);
    if(write(fd,&net_len,sizeof(net_len))!=sizeof(net_len)){
        return -1;
    }
    if(write(fd,data,len)!=(ssize_t)len){
        return -1;
    }
    return 0;
}

int recv_message(int fd,char *buffer,uint32_t *out_len){
    uint32_t net_len;
    ssize_t n = read(fd,&net_len,sizeof(net_len));
    if(n<=0){
        return -1;
    }
    uint32_t len = ntohl(net_len);
    if(len>MAX_MSG){
        return -1;
    }
    ssize_t total = 0;
    while(total<(ssize_t)len){
        ssize_t r = read(fd,buffer+total,len-total);
        if(r<=0){
            return -1;
        }
        total+=r;
    }

    buffer[len]='\0';
    *out_len = len;
    return 0;
}