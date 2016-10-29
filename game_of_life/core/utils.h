#ifndef utils_h
#define utils_h

#include <exception>
#include <iostream>
#include "game.h"

GameField getProblem(int argc, const char* argv[], unsigned &stepsCount, unsigned &threadsNumber);
void read_args(int argc, const char* argv[], size_t &height, size_t &width,
               unsigned &threads, unsigned &stepsCount, const char *&path);

#endif /* utils_h */
