#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "master.h"
#include "../core/game.h"
#include "../core/utils.h"

int init_datagram_socket(int port) {
    // создаем сокет
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Can't create socket.");
        exit(EXIT_FAILURE);
    }

    // настраиваем наш адрес
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // связываение сокета
    if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}

typedef struct Slave {
    struct sockaddr_in addr;
    socklen_t addr_size;
} Slave;

void run_master(int port, int argc, const char * argv[]) {
    printf("Master started. Port is %d!\n", port);

    // пока рабочие запускаются и подклачаются, получим задачу
    unsigned steps_count, threads_number;
    GameField field = getProblem(argc, argv, &steps_count, &threads_number);
    if (field.height < field.width) {
        transpose_field(&field);
    }

    // настраиваем соединение для рабочих
    int socket_fd = init_datagram_socket(port);

    // Фаза ожидания нужного числа рабочих
    Slave slaves[threads_number];
    size_t already_connected = 0;
    for (size_t i = 0; i < threads_number; ++i) {
        char message[255];
        // получаем заявление от рабочего
        recvfrom(socket_fd, message, 11, 0,
                 (struct sockaddr *) &slaves[already_connected].addr,
                 &slaves[already_connected].addr_size);
                 
        already_connected++;
        printf("Already connected: %zu\n", already_connected);
    }



    close(socket_fd);
    printf("Server terminated.\n");
}
//
// int receive_message(char* text, int fd, size_t length)
// {
//     size_t current = 0;
//     while (current < length)
//     {
//         int code = recv(fd, text + current, length - current, 0);
//         if (code == -1)
//         {
//             return -1;
//         }
//         current += code;
//     }
// }
