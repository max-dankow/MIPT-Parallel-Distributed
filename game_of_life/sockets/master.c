#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "master.h"

int init_socket_for_connection(int port) {
    int listen_socket = -1;

    // создаем сокет
    listen_socket = socket(AF_INET, SOCK_DGRAM, 0); // время выбрать протокол!!!!!!!!!!!!!!!!!!!!!!
    if (listen_socket == -1) {
        perror("Can't create socket.");
        exit(EXIT_FAILURE);
    }

    // связываение сокета
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    return listen_socket;
}

void run_master(int port) {
    printf("Master started. Port is %d!\n", port);

    // настраиваем соединение для приема рабочих
    int listen_socket = init_socket_for_connection(port);
    char message[255];
    recv(listen_socket, message, 11, 0);
    printf("I've received : %s\n", message);
    close(listen_socket);
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
