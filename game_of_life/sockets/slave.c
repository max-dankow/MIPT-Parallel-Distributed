#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "slave.h"
#include "socket_utils.h"

void run_slave(int port, char* master_addr_char) {
    printf("Slave started.\n");

    // создаем сокет
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);  // сокет для надежного общения с матером
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);  // сокет для обмена границами
    if (listen_socket < 0 || master_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // настраиваем прослушивающий сокет
    Address addr;
    addr.addr_size = sizeof(addr.addr);
    // struct sockaddr_in addr;
    // socklen_t addr_size = sizeof(addr);
    bzero(&addr.addr, sizeof(addr.addr));
    addr.addr.sin_family = AF_INET;
    addr.addr.sin_port = htons(0);  // порт на усмотрение ОС
    addr.addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_socket, (struct sockaddr *) &addr.addr, sizeof(addr.addr)) < 0) {
        perror("Bind");
        close(listen_socket);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    // устанавливаем сокет в режим приема новых соединений
    if (listen(listen_socket, 2) < 0) {  // число соседей всегда 2
        perror("listen");
        close(listen_socket);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    // узнаем наш действительный адрес, чтобы сообщить мастеру
    getsockname(listen_socket, (struct sockaddr *) &addr.addr, &addr.addr_size);
    printf("port %d, %d, %d(AF = %d)\n", addr.addr.sin_port,
        addr.addr.sin_addr.s_addr, addr.addr.sin_family, AF_INET);

    // настраиваем сокет для общения с мастером
    struct sockaddr_in master_addr;
    bzero(&master_addr, sizeof(master_addr));
    master_addr.sin_family = AF_INET;
    master_addr.sin_port = htons(port);
    // заполняем адрес, проверяем на корректность
    if (inet_aton(master_addr_char, &master_addr.sin_addr) == 0) {
        perror("Invalid IP address");
        close(listen_socket);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    // устанавливаем соединение с мастером
    if (connect(master_socket, (struct sockaddr *) &master_addr, sizeof(master_addr)) < 0) {
        perror("connect");
        close(listen_socket);
        close(master_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connected. Sending listen_socket addr\n");
    write(master_socket, &addr, sizeof(Address));

    printf("I've sent to my master\n");
    //
    // GameField my_field;
    // size_t height, width, steps_count;
    // read(master_socket, &steps_count, sizeof(steps_count));
    // read(master_socket, &height, sizeof(height));
    // read(master_socket, &width, sizeof(width));
    // init_field(&my_field, height + 2, width, USE_NO_VALUE);
    // read(master_socket, &my_field.data[width], sizeof(CellStatus) * height * width);
    // print_field(&my_field);
    // printf("Steps number %zu\n", steps_count);
    //
    // // получаем адреса соседей
    // Slave neighbours[2];
    // read(master_socket, &neighbours[0], sizeof(Slave));
    // read(master_socket, &neighbours[1], sizeof(Slave));
    // sendto(socket_fd, "Hello", 6, 0, (struct sockaddr *) &neighbours[0].addr, neighbours[0].addr_size);
    // sendto(socket_fd, "Hello", 6, 0, (struct sockaddr *) &neighbours[1].addr, neighbours[1].addr_size);
    // // if (
    // //     ||  < 0) {
    // //     perror("Can't write to neighbours");
    // //     close(socket_fd);
    // //     exit(EXIT_FAILURE);
    // // }
    //
    // char line[1000];
    // recvfrom(socket_fd, &line, 1000, 0, NULL, NULL);
    // printf("from sosed 0 : %s\n", line);
    //
    // bzero(&line, 10);
    // recvfrom(socket_fd, &line, 1000, 0, NULL, NULL);
    // printf("from sosed 1 : %s\n", line);
    //
    //
    //
    // // if (neighbours[0].addr_size == sizeof(my_addr) && bcmp((char *) &neighbours[0].addr, (char *) &my_addr, sizeof(my_addr)) == 0) {
    // //     printf("OK, addr is correct\n");
    // // }

    close(listen_socket);
    close(master_socket);
    printf("My work is done!\n");
}
