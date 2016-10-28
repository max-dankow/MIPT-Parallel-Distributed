#include <iostream>
#include <vector>
#include <mpi.h>
#include <unistd.h>

#include "../core/game.h"
#include "../core/utils.h"

#define SHOW_FIELD

using std::cout;
using namespace MPI;

const int MAIN_RANK = 0;

void doJob(int rank, int process_number, GameField *myField, int steps_count) {
    int myHeigth = myField->height;
    int myWidth = myField->width;
    int mySize = myHeigth * myWidth;
    GameField tmpField;
    init_field(&tmpField, myField->height, myField->width, 0);

    std::vector<CellStatus> buffer(myWidth);  // буффер для приема данных
    GameField* workFields[2] = {myField, &tmpField};

    // номера соседей
    int neighbors[2] = {(process_number + rank - 1) % process_number, (rank + 1) % process_number};

    // индексы начала и конца обоих границ нашей части поля
    int myBorders[2][2] = {{myWidth, myWidth * 2},  // верхняя граница
                           {mySize - myWidth * 2, mySize - myWidth}};  // нижняя граница

    // адреса наших границ в памяти в обоих массивах
    CellStatus *myBordersAddr[2][2] = {{myField->data + myBorders[0][0], myField->data + myBorders[1][0]},
                                       {tmpField.data + myBorders[0][0], tmpField.data + myBorders[1][0]}};

    // адреса соседских границ в памяти в обоих массивах
    CellStatus *borders[2][2] = {{myField->data, myField->data + mySize - myWidth},
                                 {tmpField.data, tmpField.data + mySize - myWidth}};

    // САМОЕ ПЕРВОЕ СООБЩЕНИЕ
    for (size_t i = 0; i < 2; ++i) {
        COMM_WORLD.Send(myBordersAddr[0][i], (int) myWidth, INT, neighbors[i], 0);
    }

    int new_index = 1;
    int old_index = 0;

    double startTime = Wtime();

    for (int i = 0; i <= steps_count; ++i) {
        // получаем любую границу
        Status status;
        COMM_WORLD.Recv(buffer.data(),
                        (int) myWidth,
                        INT,
                        ANY_SOURCE,
                        ANY_TAG, status);

        int sender = (status.Get_source() == neighbors[0]) ? 0 : 1;
        memcpy(borders[old_index][sender], buffer.data(), myWidth * sizeof(int));

        // обрабатывем соответствующую свою границу
        process_range(workFields[old_index], workFields[new_index], myBorders[sender][0], myBorders[sender][1]);

        // ответ верхнему соседу
        COMM_WORLD.Send(myBordersAddr[new_index][sender], (int) myWidth, INT, neighbors[sender], 0);

            // todo: send-receive

        // ждем другого соседа
        int other = 1 - sender;
        COMM_WORLD.Recv(borders[old_index][other], (int) myWidth, INT, neighbors[other], ANY_TAG, status);

        // обрабатывем соответствующую свою границу
        process_range(workFields[old_index], workFields[new_index], myBorders[other][0], myBorders[other][1]);

        // ответ другому соседу
        COMM_WORLD.Send(myBordersAddr[new_index][other], (int) myWidth, INT, neighbors[other], 0);

        // обрабатываем серединку
        process_range(workFields[old_index], workFields[new_index], myBorders[0][1], myBorders[1][0]);

        new_index = 1 - new_index;
        old_index = 1 - old_index;
    }
    double endTime = Wtime();
    cout << endTime - startTime << std::endl;

    // результат последнего поколения мог попасть в локальный массив,
    // поэтому нужно переместить его в результирующий массив
    if (new_index == 1) {
        move_field(std::move(tmpField), *myField);
    } else {
        destroy_field(&tmpField);
    }
}

void gameOfLifeMPI(int argc, const char * argv[]) {
    Init();
    int rank = COMM_WORLD.Get_rank();
    int process_number = COMM_WORLD.Get_size();

    // Проверяем что процессов достаточно
    if (process_number <= 2) {
        std::cerr << "Too few processes\n";
        COMM_WORLD.Abort(-1);
    }

    // в главном потоке получаем исходную задачу
    GameField initialField;
    unsigned gameHeight, gameWidth, stepsCount;
    if (rank == MAIN_RANK) {
    //    read_field(&initialField, "input.txt");
        // init_field(&initialField, gameHeight, gameWidth, USE_RANDOM);
        unsigned threadsNumber;
        initialField = getProblem(argc, argv, stepsCount, threadsNumber);
        gameHeight = initialField.height;
        gameWidth = initialField.width;
#ifdef SHOW_FIELD
        print_field(&initialField);
#endif
        cout << "START!\n";
    }

    // сообщаем число шагов реальные размеры поля
    COMM_WORLD.Bcast(&stepsCount, 1, UNSIGNED, MAIN_RANK);
    COMM_WORLD.Bcast(&gameHeight, 1, UNSIGNED, MAIN_RANK);
    COMM_WORLD.Bcast(&gameWidth, 1, UNSIGNED, MAIN_RANK);
    int gameSize = gameHeight * gameWidth;

    // распределяем куски данных
    size_t pieceHeight = gameHeight / process_number;
    size_t pieceSize = pieceHeight * gameWidth;
    std::vector<int> displacements;
    for (size_t i = 0; i < process_number; ++i) {
        displacements.push_back(int(i * pieceSize));
    }
    std::vector<int> counts(process_number, (int) pieceSize);
    *(counts.rbegin()) += gameSize - pieceSize * process_number;  // последний досчитывает остаток

    GameField myField;
    init_field(&myField, (counts[rank] / gameWidth) + 2, gameWidth, 0);  // + 2 на соседские границы
    COMM_WORLD.Scatterv(initialField.data, counts.data(), displacements.data(), INT,
                        myField.data + gameWidth, counts[rank], INT, MAIN_RANK);

    doJob(rank, process_number, &myField, stepsCount);

    // Собираем результат
    COMM_WORLD.Gatherv(myField.data + gameWidth, counts[rank], INT,
                       initialField.data, counts.data(), displacements.data(), INT, MAIN_RANK);
#ifdef SHOW_FIELD
    if (rank == MAIN_RANK) {
        cout << "RESULT IS:\n";
        print_field(&initialField);
    }
#endif

    // подчищаем память
    destroy_field(&initialField);
    destroy_field(&myField);
    Finalize();
}

int main(int argc, const char * argv[]) {
    gameOfLifeMPI(argc, argv);
    return 0;
}
