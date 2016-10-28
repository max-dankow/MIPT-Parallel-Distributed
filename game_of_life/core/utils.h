#ifndef utils_h
#define utils_h

#include "game.h"

const char* getNextWord(int argc, const char* argv[], size_t &index) {
    if (++index >= argc) {
        std::cout << index << ' ' << argc << '\n';
        perror("Wrong arguments");
        exit(EXIT_FAILURE);
    } else {
        return argv[index];
    }
}

void read_args(int argc, const char* argv[],
               size_t &height, size_t &width,
               unsigned &threads, unsigned &stepsCount,
               const char *&path) {
    threads = 2;
    stepsCount = 100;
    height = 1000;
    width = 1000;
    path = NULL;
    size_t i = 0;
    stepsCount = atoi(getNextWord(argc, argv, i));
    while (i < argc - 1) {
        const char *word = getNextWord(argc, argv, i);
        if (strcmp(word, "-np") == 0) {
            threads = atoi(getNextWord(argc, argv, i));
            continue;
        }
        if (strcmp(word, "-g") == 0) {
            height = atoi(getNextWord(argc, argv, i));
            width = atoi(getNextWord(argc, argv, i));
            continue;
        }
        if (strcmp(word, "-f") == 0) {
            path = getNextWord(argc, argv, i);
            continue;
        }
        std::cerr << "Unknown key: '" << word << "'\n";
    }
}

GameField getProblem(int argc, const char* argv[], unsigned &stepsCount, unsigned &threadsNumber) {
    GameField field;
    size_t gameHeight, gameWidth;
    const char *path;
    read_args(argc, argv, gameHeight, gameWidth, threadsNumber, stepsCount, path);
    if (path != NULL) {
        read_field(&field, path);
    } else {
        init_field(&field, gameHeight, gameWidth, USE_RANDOM);
    }
    return field;
}

#endif /* utils_h */
