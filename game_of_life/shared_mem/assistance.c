#include "assistance.h"
#include "barrier.h"
#include "../core/game.h"
#include "../core/utils.h"

typedef struct Task {
    size_t start, end;
    size_t next_end;
    unsigned steps_number;
    unsigned *assistCount;
    GameField *fields;
    thread_barrier *barrier;
    GameField **result;
} Task;

static void* job(void *arguments) {
    Task* task = (Task *) arguments;
    size_t gameSize = task->fields[0].height * task->fields[0].width;
    unsigned now_i = 0;
    unsigned next_i = 1;
    unsigned undefined_i = 2;
    unsigned assistCount = 0;  // сколько суммарно ячеек мы помогли обработать

    for (int i = 0; i < task->steps_number; ++i) {
        // обрабатываем свой участок, пока не дойдем до конца или не всретим соседа
        size_t current_cell = task->start;
        while (current_cell != task->end && task->fields[next_i].data[current_cell] == UNDEFINED) {
            update_cell(current_cell, &task->fields[now_i], &task->fields[next_i]);
            task->fields[undefined_i].data[current_cell] = UNDEFINED;
            current_cell++;
        }

        // помогаем верхнему соседу
        current_cell = (task->start + gameSize - 1) % gameSize;
        while (task->fields[next_i].data[current_cell] == UNDEFINED) {
            update_cell(current_cell, &task->fields[now_i], &task->fields[next_i]);
            task->fields[undefined_i].data[current_cell] = UNDEFINED;
            current_cell = (current_cell + gameSize - 1) % gameSize;
            assistCount++;
        }

        // помогаем нижнему
        current_cell = (task->next_end + gameSize - 1) % gameSize;
        while (task->fields[next_i].data[current_cell] == UNDEFINED) {
            update_cell(current_cell, &task->fields[now_i], &task->fields[next_i]);
            task->fields[undefined_i].data[current_cell] = UNDEFINED;
            current_cell = (current_cell + gameSize - 1) % gameSize;
            assistCount++;
        }

        // ждем всех
        int code = thread_barrier_wait(task->barrier);
        if (code == THREAD_IS_THE_LAST_ONE && i + 1 == task->steps_number) {
            (*task->result) = &task->fields[next_i];
        }
        now_i = (now_i + 1) % 3;
        next_i = (next_i + 1) % 3;
        undefined_i = (undefined_i + 1) % 3;
    }
    *task->assistCount = assistCount;
    return NULL;
}

void gameOfLifeSharedAssist(int argc, const char * argv[]) {
    // поля будет три копии: одна теущее поколение, одна следующее поколение
    // и одна будет заполняться UNDEFINED для использования через поколение
    GameField fields[3];
    unsigned steps_count, thread_number;
    fields[0] = getProblem(argc, argv, &steps_count, &thread_number);
    init_field(&fields[1], fields[0].height, fields[0].width, 0);
    init_field(&fields[2], fields[0].height, fields[0].width, 0);

    printf("[Assistance]\n");

    if (thread_number == 0) {
        return;
    }

    pthread_t threads[thread_number];
    Task tasks[thread_number];

    thread_barrier barrier;
    thread_barrier_init(&barrier, thread_number);
    GameField* result = (&fields[0]);

    size_t game_size = fields[0].height * fields[0].width;
    unsigned assists[thread_number];
    // распределяем задачи
    size_t range = game_size / thread_number;
    size_t start = 0;
    for (size_t i = 0; i < thread_number; ++i) {
        tasks[i].start = start;
        tasks[i].end = start + range;
        tasks[(i + thread_number - 1) % thread_number].next_end = tasks[i].end;
        start += range;
        tasks[i].steps_number = steps_count;
        tasks[i].fields = fields;
        tasks[i].barrier = &barrier;
        tasks[i].result = &result;
        tasks[i].assistCount = &assists[i];
    }
    // последний поток доделывает остаток (из-за деления)
    tasks[thread_number - 1].end = game_size;
    tasks[(thread_number + thread_number - 2) % thread_number].next_end = tasks[thread_number - 1].end;

    time_t time_start, time_finish;
    time(&time_start);

    for (size_t i = 0; i < thread_number; ++i) {
        pthread_create(&threads[i], NULL, &job, &tasks[i]);
    }

    unsigned totalAssists = 0;
    for (size_t i = 0; i < thread_number; ++i) {
        pthread_join(threads[i], NULL);
        totalAssists += assists[i];
    }

    time(&time_finish);
    printf("Time %ld\n", time_finish - time_start);
    printf("Total assists %d (%f %%)\n", totalAssists, (float)(totalAssists) / (float)(game_size * steps_count) * 100);

    fprint_field("assist.out", result);

    free(fields[0].data);
    free(fields[1].data);
    thread_barrier_destroy(&barrier);
}
