#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "slave.h"
#include "socket_utils.h"

void run_slave(int port, char* master_addr_char) {
    printf("Slave started.\n");

    // создаем сокет
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);  // сокет для надежного общения с матером
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);  // сокет для обмена границами
    if (socket_fd < 0 || master_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

// НАСТРОЙКА UDP

    // настраиваем наш адрес
    // заполняем нулями всю структкуру с адресом, так как она содержит ненужные нулевые поля
    struct sockaddr_in my_addr;
    socklen_t addr_size = sizeof(my_addr);
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0);  // порт на усмотрение ОС
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // привязываем сокет к адресу
    if (bind(socket_fd, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
        perror("bind");
        close(socket_fd);
        close(master_socket);
        exit(EXIT_FAILURE);
    }


// НАСТРОЙКА TCP
    // настраиваем адрес мастера
    struct sockaddr_in master_addr;
    bzero(&master_addr, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_port = htons(port);
    // заполняем адрес, проверяем на корректность
    if (inet_aton(master_addr_char, &master_addr.sin_addr) == 0) {
        perror("Invalid IP address");
        close(socket_fd);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    // устанавливаем соединение с мастером
    if (connect(master_socket, (struct sockaddr *) &master_addr, sizeof(master_addr)) < 0) {
        perror("connect");
        close(socket_fd);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connected. Sending UDP addr\n");
    write(master_socket, &addr_size, sizeof(addr_size));
    write(master_socket, &my_addr, addr_size);

    // if (sendto(socket_fd, "I'm slave!", 11, 0, (struct sockaddr *) &master_addr, sizeof(master_addr)) < 0) {
    //     perror("sendto");
    //     close(socket_fd);
    //     exit(EXIT_FAILURE);
    // }
    printf("I've sent to my master\n");

    GameField my_field;
    size_t height, width;
    read(master_socket, &height, sizeof(height));
    read(master_socket, &width, sizeof(width));
    init_field(&my_field, height + 2, width, USE_NO_VALUE);
    printf("My task is %zux%zu\n", height, width);
    read(master_socket, &my_field.data[width], sizeof(CellStatus) * height * width);
    print_field(&my_field);

    close(socket_fd);
    close(master_socket);
    printf("My work is done!\n");
}
