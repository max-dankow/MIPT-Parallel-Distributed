#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "slave.h"

void run_slave(int port, char* master_addr) {
    printf("Slave started.\n");
    //создаем сокет для общения с мастером
    int master_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (master_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    ///подключаемся к серверу
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(master_addr);
    if(connect(master_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connect");
        exit(EXIT_FAILURE);
    }
    send(master_socket, "I'm slave!", 11, 0);
    printf("I've sent to my master\n");


    close(master_socket);
    printf("My work is done!\n");
}

// int send_message(char* text, int socket, size_t length) {
//     size_t current = 0;
//     while (current < length)
//     {
//         int code = send(socket, text + current, length - current, 0);
//         if (code == -1) {
//             return -1;
//         }
//         current += code;
//     }
// }
