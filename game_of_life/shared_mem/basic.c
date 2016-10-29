#include "basic.h"
#include "barrier.h"
#include "../core/game.h"
#include "../core/utils.h"

typedef struct Task {
    size_t start, end;
    unsigned steps_number;
    GameField* fields;
    thread_barrier *barrier;
    GameField** result;
} Task;

void* job(void *arguments) {
    Task* task = (Task *) arguments;
    unsigned generation = 1;
    unsigned active_field_index = 0;

    for (int i = 0; i < task->steps_number; ++i) {
        process_range(&task->fields[active_field_index],
                      &task->fields[1 - active_field_index],
                      task->start, task->end);

        int code = thread_barrier_wait(task->barrier);
        if (code == THREAD_IS_THE_LAST_ONE && i == task->steps_number - 1) {
            (*task->result) = &task->fields[1 - active_field_index];
        }
        active_field_index = 1 - active_field_index;
        generation++;
    }
    return NULL;
}

void gameOfLifeShared(int argc, const char * argv[], bool show) {
    GameField fields[2];
    unsigned stepsCount, threadNumber;
    fields[0] = getProblem(argc, argv, stepsCount, threadNumber);
    init_field(&fields[1], fields[0].height, fields[0].width, 0);

    printf("[Basic]\n");
    if (show) {
        print_field(&fields[0]);
    }

    if (threadNumber == 0) {
        return;
    }

    pthread_t threads[threadNumber];
    Task tasks[threadNumber];

    thread_barrier barrier;
    thread_barrier_init(&barrier, threadNumber);
    GameField* result = (&fields[0]);

    size_t game_size = fields[0].height * fields[0].width;

    // распределяем задачи
    size_t range = game_size / threadNumber;
    size_t start = 0;
    for (size_t i = 0; i < threadNumber; ++i) {
        tasks[i].start = start;
        tasks[i].end = start + range;
        start += range;
        tasks[i].steps_number = stepsCount;
        tasks[i].fields = fields;
        tasks[i].barrier = &barrier;
        tasks[i].result = &result;
    }
    // последний поток доделывает остаток (из-за деления)
    tasks[threadNumber - 1].end = game_size;

    time_t time_start, time_finish;
    time(&time_start);

    for (size_t i = 0; i < threadNumber; ++i) {
        pthread_create(&threads[i], NULL, &job, &tasks[i]);
    }

    for (size_t i = 0; i < threadNumber; ++i) {
        pthread_join(threads[i], NULL);
    }

    time(&time_finish);
    std::cout << time_finish - time_start << '\n';
    thread_barrier_destroy(&barrier);

    if (show) {
        print_field(result);
    }

    free(fields[0].data);
    free(fields[1].data);
}
