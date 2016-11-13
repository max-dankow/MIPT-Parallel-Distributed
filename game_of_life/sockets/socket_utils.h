#ifndef socket_utils_h
#define socket_utils_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../core/game.h"

// extern const size_t MAX_BOARDER_SIZE;

typedef struct Slave {
    struct sockaddr_in addr;
    socklen_t addr_size;
    int tcp_socket;
} Slave;

int send_message(char* text, int fd, size_t length);
int receive_message(char* text, int fd, size_t length);

#endif /* socket_utils_h */
