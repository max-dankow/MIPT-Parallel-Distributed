#include <string.h>
#include "utils.h"

static const char* get_next_word(int argc, const char* argv[], size_t *index) {
    (*index)++;
    if (*index >= argc) {
        perror("Wrong arguments");
        exit(EXIT_FAILURE);
    } else {
        return argv[*index];
    }
}

void read_args(int argc, const char* argv[],
               size_t *height,
               size_t *width,
               unsigned *threads,
               unsigned *stepsCount,
               const char **path) {
    *threads = 4;
    *stepsCount = 100;
    *height = 10;
    *width = 10;
    *path = NULL;
    size_t i = 0;
    while (i < argc - 1) {
        const char *word = get_next_word(argc, argv, &i);
        if (strcmp(word, "-np") == 0) {
            *threads = atoi(get_next_word(argc, argv, &i));
            continue;
        }
        if (strcmp(word, "-g") == 0) {
            *height = atoi(get_next_word(argc, argv, &i));
            *width = atoi(get_next_word(argc, argv, &i));
            continue;
        }
        if (strcmp(word, "-f") == 0) {
            *path = get_next_word(argc, argv, &i);
            continue;
        }
        if (strcmp(word, "--help") == 0) {
            printf("Usage: executable <steps count>\n"
                    "Flags: -g <height> <width> - generate random field\n"
                           "-f <path to file> - read field from file\n"
                           "-np <process number>\n");
            return;
        }
        if (i == 1) {
            *stepsCount = atoi(word);
            continue;
        }
        perror("Unknown key");
    }
}

GameField getProblem(int argc, const char* argv[], unsigned *stepsCount, unsigned *threadsNumber) {
    GameField field;
    size_t gameHeight, gameWidth;
    const char *path;
    read_args(argc, argv, &gameHeight, &gameWidth, threadsNumber, stepsCount, &path);
    if (path != NULL) {
        read_field(&field, path);
    } else {
        init_field(&field, gameHeight, gameWidth, USE_RANDOM);
    }
    return field;
}
