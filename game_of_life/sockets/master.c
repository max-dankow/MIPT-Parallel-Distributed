#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "master.h"
#include "../core/game.h"
#include "../core/utils.h"
#include "socket_utils.h"

// int init_datagram_socket(int port) {
//     // создаем сокет
//     int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (socket_fd < 0) {
//         perror("Can't create socket.");
//         exit(EXIT_FAILURE);
//     }
//
//     // настраиваем наш адрес
//     struct sockaddr_in addr;
//     bzero(&addr, sizeof(addr));
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     // связываение сокета
//     if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
//         perror("Bind");
//         close(socket_fd);
//         exit(EXIT_FAILURE);
//     }
//     return socket_fd;
// }


void close_all_sockets(Slave slaves[], size_t threads_number) {
    for (size_t i = 0; i < threads_number; ++i) {
        close(slaves[i].socket);
    }
}

void find_slaves(Slave slaves[], size_t slaves_number, int port) {
    printf("Waiting for %zu slaves\n", slaves_number);
    // настраиваем соединение для приема рабочих (TCP)
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0) {
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
    if (bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Bind");
        close(listen_socket);
        exit(EXIT_FAILURE);
    }
    // устанавливаем сокет в режим приема новых соединений
    if (listen(listen_socket, slaves_number) < 0) {
        perror("listen");
        close(listen_socket);
        exit(EXIT_FAILURE);
    }

    // Фаза ожидания нужного числа рабочих
    for (size_t i = 0; i < slaves_number; ++i) {
        slaves[i].socket = accept(listen_socket, NULL, NULL);
        if (slaves[i].socket < 0) {
            perror("accept");
            close(listen_socket);
            exit(EXIT_FAILURE);
        }
        printf("already_connected %zu of %zu\n", i + 1, slaves_number);

        // рабочий должен прислать адрес его сокета для приема соединений с соседями
        // внесем его в список рабочих, запомним этот адрес
        if (read(slaves[i].socket, &slaves[i].listen_addrs, sizeof(Address)) != sizeof(Address)) {
            perror("Address expected");
            close(listen_socket);
            close_all_sockets(slaves, i);
            exit(EXIT_FAILURE);
        }
    }
    // теперь не ожидается новых подключений и можно закрыть сокет
    close(listen_socket);
}
//
// void scatter_tasks(Slave slaves[], GameField *field, size_t threads_number, size_t steps_count) {
//     size_t height = field->height;
//     size_t width = field->width;
//     size_t game_size = height * width;
//     size_t piece_height = height / threads_number;
//     size_t piece_size = piece_height * width;
//     for (size_t i = 0; i < threads_number; ++i) {
//         // отсылаем число шагов
//         write(slaves[i].tcp_socket, &steps_count, sizeof(steps_count));
//         // размер подполя для последнего отличается из-за некратности
//         size_t actual_size = (i + 1 < threads_number) ? piece_size : (game_size - i * piece_size);
//         size_t actual_height = actual_size / width;
//
//         // отсылаем размер последующего куска данных
//         write(slaves[i].tcp_socket, &actual_height, sizeof(actual_height));
//         write(slaves[i].tcp_socket, &width, sizeof(width));
//
//         // отсылаем подполе
//         if (send_message((char *) &(field->data[i * piece_size]),
//                          slaves[i].tcp_socket,
//                          actual_size * sizeof(CellStatus)) < 0) {
//             perror("Send");
//             close_all_sockets(slaves, threads_number);
//             exit(EXIT_FAILURE);
//         }
//         // остылаем адреса соседей (поле сокет окажется невалидным, но оно и не нужно)
//         size_t neighbour_left = (i + threads_number - 1) % threads_number;
//         size_t neighbour_right = (i + 1) % threads_number;
//         if (send_message((char *) &(slaves[neighbour_left]), slaves[i].tcp_socket, sizeof(slaves[neighbour_left])) < 0
//             || send_message((char *) &(slaves[neighbour_right]), slaves[i].tcp_socket, sizeof(slaves[neighbour_right])) < 0) {
//             perror("Send");
//             close_all_sockets(slaves, threads_number);
//             exit(EXIT_FAILURE);
//         }
//     }
// }

void run_master(int port, int argc, const char * argv[]) {
    printf("Master started. Port is %d!\n", port);

    // пока рабочие запускаются и подклачаются, получим задачу
    unsigned steps_count, threads_number;
    GameField field = getProblem(argc, argv, &steps_count, &threads_number);
    // if (threads_number <= 2) {
    //     perror("To few processes");
    //     exit(EXIT_FAILURE);
    // }
    if (field.height < field.width) {
        transpose_field(&field);
    }
    print_field(&field);
    Slave slaves[threads_number];
    find_slaves(slaves, threads_number, port);
    printf("Slaves have been found\n");

    // scatter_tasks(slaves, &field, threads_number, steps_count);
    // printf("Tasks have been scattered\n");
    printf("Server terminated.\n");
}
