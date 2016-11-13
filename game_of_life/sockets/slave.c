#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "slave.h"

void run_slave(int port, char* master_addr_char) {
    printf("Slave started.\n");
    
    // создаем сокет
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // настраиваем наш адрес
    // заполняем нулями всю структкуру с адресом, так как она содержит ненужные нулевые поля
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0);  // порт на усмотрение ОС
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // привязываем сокет к адресу
    if (bind(socket_fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("bind");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    // настраиваем адрес мастера
    struct sockaddr_in master_addr;
    bzero(&master_addr, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_port = htons(port);
    // заполняем адрес, проверяем на корректность
    if (inet_aton(master_addr_char, &master_addr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(socket_fd);
        exit(1);
    }

    if (sendto(socket_fd, "I'm slave!", 11, 0, (struct sockaddr *) &master_addr, sizeof(master_addr)) < 0) {
        perror("sendto");
        close(socket_fd);
        exit(1);
    }
    printf("I've sent to my master\n");

    char answer[100];
    recvfrom(socket_fd, &answer, 100, 0, NULL, NULL);
    printf("%s\n", answer);




    close(socket_fd);
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
