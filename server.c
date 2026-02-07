#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#include "protocol.h"
#include "client.h"

#define MAX_CLIENTS  FD_SETSIZE

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen"); return 1;
    }

    set_nonblocking(server_fd);

    printf("Server listening on port 8080...\n");

    client_t clients[MAX_CLIENTS] = {0};

    fd_set master, read_fds;
    FD_ZERO(&master);
    FD_SET(server_fd, &master);
    int fd_max = server_fd;

    while (1) {
        read_fds = master;
        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select"); return 1;
        }

        for (int fd = 0; fd <= fd_max; fd++) {
            if (!FD_ISSET(fd, &read_fds)) continue;

            if (fd == server_fd) {
                // New connection(s)
                while (1) {
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept"); break;
                    }

                    set_nonblocking(client_fd);
                    FD_SET(client_fd, &master);
                    if (client_fd > fd_max) fd_max = client_fd;

                    clients[client_fd].fd = client_fd;
                    clients[client_fd].buf_used = 0;

                    printf("New client %d connected\n", client_fd);
                }
            } else {
                // Existing client has data
                client_t *c = &clients[fd];
                int done = 0;

                while (1) {
                    ssize_t r = read(fd, c->buf + c->buf_used, BUF_SIZE - c->buf_used);
                    if (r > 0) {
                        c->buf_used += r;

                        // Process full messages inside buffer
                        int offset = 0;
                        while (c->buf_used - offset >= 4) {
                            uint32_t net_len;
                            memcpy(&net_len, c->buf + offset, 4);
                            uint32_t len = ntohl(net_len);

                            if (len > MAX_MSG) { done = 1; break; }
                            if (c->buf_used - offset < (int)(4 + len)) break;

                            char msg[MAX_MSG + 1];
                            memcpy(msg, c->buf + offset + 4, len);
                            msg[len] = '\0';

                            printf("Client %d: %s\n", fd, msg);
                            send_message(fd, msg, len);

                            offset += 4 + len;
                        }

                        if (offset > 0) {
                            memmove(c->buf, c->buf + offset, c->buf_used - offset);
                            c->buf_used -= offset;
                        }
                    } else if (r == 0) {
                        done = 1;
                        break;
                    } else {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        done = 1;
                        break;
                    }
                }

                if (done) {
                    printf("Client %d disconnected\n", fd);
                    close(fd);
                    FD_CLR(fd, &master);
                    clients[fd].fd = 0;
                    clients[fd].buf_used = 0;
                }
            }
        }
    }

    close(server_fd);
    return 0;
}