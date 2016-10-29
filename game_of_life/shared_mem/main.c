#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "basic.h"
#include "assistance.h"

int main(int argc, const char* argv[]) {
    gameOfLifeShared(argc, argv);
    gameOfLifeSharedAssist(argc, argv);
	return 0;
}
