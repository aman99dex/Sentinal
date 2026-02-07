#ifndef NETIO_H
#define NETIO_H

#include<stddef.h>
#include<unistd.h>
#include<sys/types.h>

ssize_t readn(int fd,void *buf,size_t n);
ssize_t writen(int fd,const void *buf,size_t n);

#endif