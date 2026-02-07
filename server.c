#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "protocol.h"

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

    printf("Server listening on port 8080...\n");

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
                // New client connection
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) { perror("accept"); continue; }

                FD_SET(client_fd, &master);
                if (client_fd > fd_max) fd_max = client_fd;

                printf("New client %d connected\n", client_fd);
            } else {
                // Existing client sent data
                char buffer[MAX_MSG + 1];
                uint32_t len;

                if (recv_message(fd, buffer, &len) == 0) {
                    printf("Client %d: %s\n", fd, buffer);
                    send_message(fd, buffer, len);
                } else {
                    printf("Client %d disconnected\n", fd);
                    close(fd);
                    FD_CLR(fd, &master);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}