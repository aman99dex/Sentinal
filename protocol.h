#ifndef PROTOCOL_H
#define PROTOCOL_H

#include<stdint.h>

#define MAX_MSG 1024

int send_message(int fd,const char *data,uint32_t len);
int recv_message(int fd,char *buffer,uint32_t *out_len);

#endif