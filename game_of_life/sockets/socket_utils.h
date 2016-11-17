#ifndef socket_utils_h
#define socket_utils_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../core/game.h"

typedef struct Address {
    struct sockaddr_in addr;
    socklen_t addr_size;
} Address;

typedef struct Slave {
    Address listen_addr;  // адреса сокета прослушивающего сокета рабочего
    int socket;  // сокет, по которому мастер общается с рабочим
} Slave;

int send_message(int fd, char* text, size_t length);
int receive_message(int fd, char* text, size_t length);

#endif /* socket_utils_h */
