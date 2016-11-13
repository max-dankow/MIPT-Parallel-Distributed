#include "socket_utils.h"
//
// const size_t MAX_BOARDER_SIZE = 100000;
//
int send_message(char* text, int fd, size_t length) {
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

int receive_message(char* text, int fd, size_t length) {
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
