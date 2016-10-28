#ifndef utils_h
#define utils_h

#include <exception>
#include "game.h"

const char* getNextWord(int argc, const char* argv[], size_t &index) {
    if (++index >= argc) {
        throw(std::invalid_argument("Wrong arguments"));
    } else {
        return argv[index];
    }
}

void read_args(int argc, const char* argv[],
               size_t &height, size_t &width,
               unsigned &threads, unsigned &stepsCount,
               const char *&path) {
    threads = 4;
    stepsCount = 100;
    height = 10;
    width = 10;
    path = NULL;
    size_t i = 0;
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
        if (strcmp(word, "--help") == 0) {
            std::cout << "Usage: mpi_game <steps count>\n"
                      << "Flags: -g <height> <width> - generate random field\n"
                      << "       -f <path to file> - read field from file\n"
                      << "       -np <process number>\n";
            return;
        }
        if (i == 0) {
            stepsCount = atoi(getNextWord(argc, argv, i));
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
