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
        if (read(slaves[i].socket, &slaves[i].listen_addr, sizeof(Address)) != sizeof(Address)) {
            perror("Address expected");
            close(listen_socket);
            close_all_sockets(slaves, i);
            exit(EXIT_FAILURE);
        }
    }
    // теперь не ожидается новых подключений и можно закрыть сокет
    close(listen_socket);
}

void scatter_tasks(Slave slaves[], GameField *field, size_t threads_number, size_t steps_count) {
    size_t height = field->height;
    size_t width = field->width;
    size_t game_size = height * width;
    size_t piece_height = height / threads_number;
    size_t piece_size = piece_height * width;
    for (size_t i = 0; i < threads_number; ++i) {
        // отсылаем ID рабочего
        write(slaves[i].socket, &i, sizeof(i));
        write(slaves[i].socket, &threads_number, sizeof(threads_number));
        // отсылаем число шагов
        write(slaves[i].socket, &steps_count, sizeof(steps_count));
        // размер подполя для последнего отличается из-за некратности
        size_t actual_size = (i + 1 < threads_number) ? piece_size : (game_size - i * piece_size);
        size_t actual_height = actual_size / width;

        // отсылаем размеры последующего отправляемой части поля
        write(slaves[i].socket, &actual_height, sizeof(actual_height));
        write(slaves[i].socket, &width, sizeof(width));

        // отсылаем подполе
        if (send_message(slaves[i].socket,
                         (char *) &(field->data[i * piece_size]),
                         actual_size * sizeof(CellStatus)) < 0) {
            perror("Send");
            close_all_sockets(slaves, threads_number);
            exit(EXIT_FAILURE);
        }

        // отсылаем информацию о соседях
        size_t neighbour_left = (i + threads_number - 1) % threads_number;
        size_t neighbour_right = (i + 1) % threads_number;
        if (send_message(slaves[i].socket, (char *) &(slaves[neighbour_left].listen_addr), sizeof(Address)) < 0
            || send_message(slaves[i].socket, (char *) &(slaves[neighbour_right].listen_addr), sizeof(Address)) < 0) {
            perror("Send");
            close_all_sockets(slaves, threads_number);
            exit(EXIT_FAILURE);
        }
    }
}

// Собирает результат из ответов рабочих.
// Поле result должно быть проинициализировно, размеры должны совпадать с размерами результатов.
void gather_result(GameField *result, Slave slaves[], size_t threads_number) {
    size_t game_size = result->height * result->width;
    size_t piece_height = result->height / threads_number;
    size_t piece_size = piece_height * result->width;
    for (size_t i = 0; i < threads_number; ++i) {
        CellStatus *dest = &(result->data[i * piece_size]);
        size_t actual_size = (i + 1 < threads_number) ? piece_size : (game_size - i * piece_size);
        receive_message(slaves[i].socket, dest, actual_size * sizeof(CellStatus));
    }
}

void run_master(int port, int argc, const char * argv[]) {
    printf("Master started. Port is %d!\n", port);

    // пока рабочие запускаются и подклачаются, получим задачу
    unsigned steps_count, threads_number;
    GameField field = getProblem(argc, argv, &steps_count, &threads_number);
    if (threads_number <= 2) {
        perror("To few processes");
        exit(EXIT_FAILURE);
    }
    if (field.height < field.width) {
        transpose_field(&field);
    }
    print_field(&field);
    Slave slaves[threads_number];
    find_slaves(slaves, threads_number, port);
    printf("Slaves have been found\n");

    scatter_tasks(slaves, &field, threads_number, steps_count);
    printf("Tasks have been scattered\n");

    gather_result(&field, slaves, threads_number);
    printf("Resut has been received:\n");
    print_field(&field);

    close_all_sockets(slaves, threads_number);
    printf("Server terminated.\n");
}
