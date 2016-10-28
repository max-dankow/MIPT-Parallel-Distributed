#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "basic.h"
//
// void* job_improved(void *arguments) {
//     Task* task = (Task *) arguments;
//     unsigned generation = 1;
//     unsigned active_field_index = 0;
//
//     for (int i = 0; i < task->steps_number; ++i) {
//         process_range(&task->fields[active_field_index],
//                       &task->fields[1 - active_field_index],
//                       task->start, task->end);
//
//         int code = thread_barrier_wait(task->barrier);
//
//         if (code == THREAD_IS_THE_LAST_ONE && i == task->steps_number - 1) {
//             // printf("GENERATION %u (i'm %zu)\n", generation, task->start);
//             // print_field();
//             (*task->result) = &task->fields[1 - active_field_index];
//         }
//         active_field_index = 1 - active_field_index;
//         generation++;
//     }
//     return NULL;
// }



int main(int argc, const char* argv[]) {
    // unsigned stepsCount, thread_number;
    //
    // GameField fields[2];
    // fields[0] = getProblem(argc, argv, stepsCount, thread_number);
    // init_field(&fields[1], fields[0].height, fields[0].width, 0);
    //
    // printf("GENETARION 0 (i'm the main)\n");
    // print_field(&fields[0]);
    time_t time_start, time_finish;
    time(&time_start);
    gameOfLifeShared(argc, argv);
    time(&time_finish);
    std::cout << time_finish - time_start << '\n';
    //
    // free(fields[0].data);
    // free(fields[1].data);
	return 0;
}
