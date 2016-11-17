#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "slave.h"
#include "socket_utils.h"

void connect_with_neighbours(Slave neighbours[], int listen_socket, size_t my_id, size_t threads_number) {
    // ТУТ НУЖЕН ПРАВИЛЬНЫЙ ПОРЯДОК: при нечетном числе исполнителей нулевой и последний должны изменить свое поведение
    if (my_id % 2 == 0) {
        // В этом случае я сначала жду подключения. Сначала снизу, затем сверху.
        neighbours[1].socket = accept(listen_socket, NULL, NULL);
        printf("First accept %d\n", neighbours[1].socket);
        if (threads_number % 2 == 1) {
            if (my_id == 0) {
                // В этом случае 0ой вместо ожидания верхнего должен подключиться к нему.
                neighbours[0].socket = socket(AF_INET, SOCK_STREAM, 0);
                if (connect(neighbours[0].socket,
                            (struct sockaddr *) &neighbours[0].listen_addr.addr,
                            neighbours[0].listen_addr.addr_size) < 0) {
                    perror("connect");
                    close(listen_socket);
                    close(neighbours[0].socket);
                    close(neighbours[1].socket);
                    exit(EXIT_FAILURE);
                }
                printf("EXTRA connect in OK\n");
            }
            if (my_id == threads_number - 1) {
                // В этом случае последний получил сначала подкличение от верхнего, а нужно наоборот.
                neighbours[0].socket = neighbours[1].socket;
                // ожидаем подключения от 0го
                neighbours[1].socket = accept(listen_socket, NULL, NULL);
                if (neighbours[1].socket < 0) {
                    perror("accept");
                    close(listen_socket);
                    close(neighbours[0].socket);
                    close(neighbours[1].socket);
                    exit(EXIT_FAILURE);
                }
                printf("EXTRA accept %d\n", neighbours[1].socket);
            }
        } else {
            neighbours[0].socket = accept(listen_socket, NULL, NULL);
            printf("Second accept %d\n", neighbours[0].socket);
        }
        if (neighbours[0].socket < 0 || neighbours[1].socket < 0) {
            perror("accept");
            close(listen_socket);
            close(neighbours[0].socket);
            close(neighbours[1].socket);
            exit(EXIT_FAILURE);
        }
    } else {
        // В этом случае мы инициируем подключение. Сначала с верхним, потом с нижним.
        for (size_t i = 0; i < 2; ++i) {
            neighbours[i].socket = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(neighbours[i].socket,
                        (struct sockaddr *) &neighbours[i].listen_addr.addr,
                        neighbours[i].listen_addr.addr_size) < 0) {
                perror("connect");
                close(listen_socket);
                close(neighbours[0].socket);
                close(neighbours[1].socket);
                exit(EXIT_FAILURE);
            }
            printf("%zuth connect in OK\n", i);
        }
    }
}

void run_slave(int port, char* master_addr_char) {
    printf("Slave started.\n");

    // создаем сокет
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);  // сокет для надежного общения с мастером
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

    // узнаем наш действительный адрес, чтобы сообщить мастеру
    getsockname(listen_socket, (struct sockaddr *) &addr.addr, &addr.addr_size);
    printf("port %d, %d, %d(AF = %d)\n", addr.addr.sin_port,
        addr.addr.sin_addr.s_addr, addr.addr.sin_family, AF_INET);
    write(master_socket, &addr, sizeof(Address));
    printf("I've sent address to my master\n");

    // получаем задание от мастера
    GameField my_field;
    size_t height, width, steps_count, my_id, threads_number;
    read(master_socket, &my_id, sizeof(my_id));
    read(master_socket, &threads_number, sizeof(threads_number));
    read(master_socket, &steps_count, sizeof(steps_count));
    read(master_socket, &height, sizeof(height));
    read(master_socket, &width, sizeof(width));
    init_field(&my_field, height + 2, width, USE_NO_VALUE);  // + по одной строке на верхнего и нижнего соседа
    read(master_socket, &my_field.data[width], sizeof(CellStatus) * height * width);
    print_field(&my_field);
    printf("My ID=%zu(of %zu). Steps number %zu\n", my_id, threads_number, steps_count);

    // получаем адреса соседей
    Slave neighbours[2];
    read(master_socket, &neighbours[0].listen_addr, sizeof(Address));
    read(master_socket, &neighbours[1].listen_addr, sizeof(Address));

    // устанавливаем соединение с соседями
    connect_with_neighbours(neighbours, listen_socket, my_id, threads_number);

    // проверка связи
    char msg[2][100];
    sprintf(msg[0], "For %zu from %zu with love of TCP", (my_id + threads_number - 1) % threads_number, my_id);
    sprintf(msg[1], "For %zu from %zu with love of TCP", (my_id + 1) % threads_number, my_id);
    for (size_t i = 0; i < 2; ++i) {
        send_message(neighbours[i].socket, msg[i], strlen(msg[i]) + 1);
    }
    char line[2][100];
    for (size_t i = 0; i < 2; ++i) {
        receive_message(neighbours[i].socket, line[i], strlen(msg[i]) + 1);
        printf("%s\n", line[i]);
    }

    close(listen_socket);
    close(master_socket);
    printf("My work is done!\n");
}
