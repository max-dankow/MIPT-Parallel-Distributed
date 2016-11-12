#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "basic.h"
#include "assistance.h"
#include "assist_all.h"

int main(int argc, const char* argv[]) {
    gameOfLifeShared(argc, argv);
    gameOfLifeSharedAssist(argc, argv);
    gameOfLifeSharedAssistAll(argc, argv);
    return 0;
}
