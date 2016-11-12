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

void gameOfLifeShared(int argc, const char * argv[]) {
    GameField fields[2];
    unsigned steps_count, thread_number;
    fields[0] = getProblem(argc, argv, &steps_count, &thread_number);
    init_field(&fields[1], fields[0].height, fields[0].width, 0);

    printf("[Basic]\n");

    if (thread_number == 0) {
        return;
    }

    pthread_t threads[thread_number];
    Task tasks[thread_number];

    thread_barrier barrier;
    thread_barrier_init(&barrier, thread_number);
    GameField* result = (&fields[0]);

    size_t game_size = fields[0].height * fields[0].width;

    // распределяем задачи
    size_t range = game_size / thread_number;
    size_t start = 0;
    for (size_t i = 0; i < thread_number; ++i) {
        tasks[i].start = start;
        tasks[i].end = start + range;
        start += range;
        tasks[i].steps_number = steps_count;
        tasks[i].fields = fields;
        tasks[i].barrier = &barrier;
        tasks[i].result = &result;
    }
    // последний поток доделывает остаток (из-за деления)
    tasks[thread_number - 1].end = game_size;

    time_t time_start, time_finish;
    time(&time_start);

    for (size_t i = 0; i < thread_number; ++i) {
        pthread_create(&threads[i], NULL, &job, &tasks[i]);
    }

    for (size_t i = 0; i < thread_number; ++i) {
        pthread_join(threads[i], NULL);
    }

    time(&time_finish);
    printf("Time %ld\n", time_finish - time_start);
    thread_barrier_destroy(&barrier);

    free(fields[0].data);
    free(fields[1].data);
}
