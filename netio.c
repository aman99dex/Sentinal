#include "netio.h"
#include <unistd.h>
#include <errno.h>

size_t readn(int fd, void *buf, size_t n) {
    size_t total = 0;
    char *p = buf;
    while (total < n) {
        ssize_t r = read(fd, p + total, n - total);
        if (r < 0) {
            if (errno == EINTR) continue; // Interrupted, try again
            return -1; // Error
        }
        if (r == 0) return total; // EOF
        total += r;
    }
    return total;
}

ssize_t writen(int fd, const void *buf, size_t n) {
    size_t total = 0;
    const char *p = buf;
    while (total < n) {
        ssize_t w = write(fd, p + total, n - total);
        if (w <= 0) {
            if (w<0 &&errno == EINTR) continue; // Interrupted, try again
            return -1; // Error
        }
        total += w;
    }
    return total;
}