#ifndef game_h
#define game_h

#include <stdio.h>
#include <stdlib.h>

extern const int USE_RANDOM;

typedef enum CellStatus {
    DEAD,
    ALIVE,
    UNDEFINED
} CellStatus;

typedef struct GameField {
   CellStatus* data;
   size_t height, width;
} GameField;

void init_field(GameField *field, size_t height, size_t width, int random_flag);
void destroy_field(GameField *field);

void read_field(GameField *field, const char* path);
void move_field(GameField &source, GameField &dest);
void transpose_field(GameField *field);

void print_field(const GameField * field);
void fprint_field(FILE* file, const GameField * field);

CellStatus process_cell(size_t index, const GameField *field);
void update_cell(size_t index, const GameField *before, GameField *after);
void process_range(const GameField *old_field, GameField *new_field, size_t start, size_t end);

#endif /* game_h */
