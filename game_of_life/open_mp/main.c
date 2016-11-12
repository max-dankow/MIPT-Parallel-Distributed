#include <stdio.h>
#include <omp.h>

#include "../core/game.h"
#include "../core/utils.h"

int main(int argc, const char* argv[]) {
    // получение задания
    GameField fields[2];
    GameField *result = fields;
    unsigned stepsCount, threadNumber;
    fields[0] = getProblem(argc, argv, &stepsCount, &threadNumber);
    init_field(&fields[1], fields[0].height, fields[0].width, 0);
    size_t gameHeight = fields[0].height;
    size_t gameWidth = fields[0].width;

    printf("START!\n");

    // будущие локальные переменные
    unsigned active_field_index = 0;

    // разделение на потоки и выполнение
    double timeStart = omp_get_wtime();
    #pragma omp parallel num_threads(threadNumber) shared(fields, stepsCount) firstprivate(active_field_index)
    {
        for (int step = 0; step < stepsCount; ++step) {
            #pragma omp for
            for (size_t row = 0; row < gameHeight; ++row) {
                process_range(&fields[active_field_index],
                              &fields[1 - active_field_index],
                              row * gameWidth, row * gameWidth + gameWidth);
            }  // неявная барьерная синхронизация
            active_field_index = 1 - active_field_index;
            if (step + 1 == stepsCount) {
                result = &fields[1 - active_field_index];
            }
        }
    }
    double timeFinish = omp_get_wtime();
    printf("Time %f\n", timeFinish - timeStart);
    return 0;
}
