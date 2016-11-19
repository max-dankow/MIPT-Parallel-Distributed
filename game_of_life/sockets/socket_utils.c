#include "socket_utils.h"

int send_message(int fd, void* text, size_t length) {
    size_t current = 0;
    while (current < length) {
        int code = send(fd, text + current, length - current, 0);
        if (code == -1) {
            return -1;
        }
        current += code;
    }
    return length;
}

int receive_message(int fd, void* text, size_t length) {
    size_t current = 0;
    while (current < length) {
        int code = recv(fd, text + current, length - current, 0);
        if (code == -1) {
            return -1;
        }
        current += code;
    }
    return length;
}
