#ifndef CLIENT_H
#define CLIENT_H

#include<stdint.h>
#define BUF_SIZE 4096

typedef struct {
    int fd;
    uint8_t buf[BUF_SIZE];
    int buf_used;
} client_t;

#endif