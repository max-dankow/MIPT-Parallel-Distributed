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
    // узнаем наш действительный адрес, чтобы сообщить мастеру
    getsockname(socket_fd, (struct sockaddr *) &my_addr, &addr_size);
    printf("port %d, %d, %d(AF = %d)\n", my_addr.sin_port,
           my_addr.sin_addr.s_addr, my_addr.sin_family, AF_INET);

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

    printf("I've sent to my master\n");

    GameField my_field;
    size_t height, width, steps_count;
    read(master_socket, &steps_count, sizeof(steps_count));
    read(master_socket, &height, sizeof(height));
    read(master_socket, &width, sizeof(width));
    init_field(&my_field, height + 2, width, USE_NO_VALUE);
    read(master_socket, &my_field.data[width], sizeof(CellStatus) * height * width);
    print_field(&my_field);
    printf("Steps number %zu\n", steps_count);

    // получаем адреса соседей
    Slave neighbours[2];
    read(master_socket, &neighbours[0], sizeof(Slave));
    read(master_socket, &neighbours[1], sizeof(Slave));
    sendto(socket_fd, "Hello", 6, 0, (struct sockaddr *) &neighbours[0].addr, neighbours[0].addr_size);
    sendto(socket_fd, "Hello", 6, 0, (struct sockaddr *) &neighbours[1].addr, neighbours[1].addr_size);
    // if (
    //     ||  < 0) {
    //     perror("Can't write to neighbours");
    //     close(socket_fd);
    //     exit(EXIT_FAILURE);
    // }

    char line[1000];
    recvfrom(socket_fd, &line, 1000, 0, NULL, NULL);
    printf("from sosed 0 : %s\n", line);

    bzero(&line, 10);
    recvfrom(socket_fd, &line, 1000, 0, NULL, NULL);
    printf("from sosed 1 : %s\n", line);



    // if (neighbours[0].addr_size == sizeof(my_addr) && bcmp((char *) &neighbours[0].addr, (char *) &my_addr, sizeof(my_addr)) == 0) {
    //     printf("OK, addr is correct\n");
    // }

    close(socket_fd);
    close(master_socket);
    printf("My work is done!\n");
}
