#include <time.h>
#include "game.h"

const int USE_UNDEFINED = 0;
const int USE_RANDOM = 1;
const int USE_NO_VALUE = 2;

size_t get_index(size_t row, size_t col, const GameField *field) {
    return row * field->width + col;
}

void init_field(GameField *field, size_t height, size_t width, int init_mode) {
    if (init_mode == USE_RANDOM) {
        srand(time(NULL));
    }

    field->height = height;
    field->width = width;
    size_t field_size = height * width;
    field->data = (CellStatus*) malloc(sizeof(CellStatus) * field_size);

    for (size_t index = 0; index < field_size; ++index) {
        if (init_mode == USE_UNDEFINED) {
            field->data[index] = UNDEFINED;
        }
        if (init_mode == USE_RANDOM) {
            if (rand() % 2) {
                field->data[index] = DEAD;
            } else {
                field->data[index] = ALIVE;
            }
        }
    }
}

void destroy_field(GameField *field) {
    free(field->data);
    field->data = NULL;
}

void move_field(GameField *source, GameField *dest) {
    dest->height = source->height;
    dest->width = source->width;
    destroy_field(dest);
    dest->data = source->data;
    source->data = NULL;
}

void transpose_field(GameField *field) {
    GameField transposed;
    init_field(&transposed, field->width, field->height, USE_NO_VALUE);
    for (size_t row = 0; row < field->height; ++row) {
        for (size_t col = 0; col < field->width; ++col) {
            transposed.data[col * transposed.width + row] = field->data[row * field->width + col];
        }
    }
    move_field(&transposed, field);
}

void read_field(GameField *field, const char* path) {
    FILE* input = fopen(path, "r");

    if (input == NULL) {
        perror("Can't open file");
        exit(EXIT_FAILURE);
    }

    fscanf(input, "%zu %zu", &field->height, &field->width);
    field->data = (CellStatus*) malloc(sizeof(CellStatus) * field->height * field->width);

    size_t current = 0;
    for (size_t row = 0; row < field->height; ++row) {
        for (size_t col = 0; col < field->width; ++col) {
            int code;
            fscanf(input, "%d", &code);

            CellStatus cell = UNDEFINED;
            if (code == 0) cell = DEAD;
            if (code == 1) cell = ALIVE;

            field->data[current] = cell;
            ++current;
        }
    }
    fclose(input);
}

void print_field(const GameField * field) {
    size_t index = 0;
    for (size_t row = 0; row < field->height; ++row) {
        for (size_t col = 0; col < field->width; ++col) {
            CellStatus CellStatus = field->data[index];
            index++;
            char c = '?';
            if (CellStatus == ALIVE) {
                c = '*';
            }
            if (CellStatus == DEAD) {
                c = '.';
            }
            printf("%c", c);
        }
        printf("\n");
    }
    printf("\n");
}

void fprint_field(const char* file_name, const GameField * field) {
    FILE *file = fopen(file_name, "w");
    size_t index = 0;
    fprintf(file, "%zu %zu\n", field->height, field->width);
    for (size_t row = 0; row < field->height; ++row) {
        for (size_t col = 0; col < field->width; ++col) {
            CellStatus CellStatus = field->data[index];
            index++;
            fprintf(file, "%c ", (CellStatus == ALIVE) ? '1' : '0');
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
    fclose(file);
}

CellStatus check_cell(int row, int col, const GameField *field) {
    row = (row + field->height) % field->height;
    col = (col + field->width) % field->width;
    size_t index = get_index(row, col, field);
    return field->data[index];
}

CellStatus process_cell(size_t index, const GameField *field) {
    size_t row = index / field->width;
    size_t col = index % field->width;

    // пересчитываеем живых соседей
    size_t neighbors_count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (check_cell(row + i, col + j, field) == ALIVE) {
                neighbors_count++;
            }
        }
    }

    // применяем правила игры
    CellStatus current_CellStatus = field->data[get_index(row, col, field)];
    CellStatus new_CellStatus = DEAD;
    if ((current_CellStatus == DEAD && neighbors_count == 3) || (current_CellStatus == ALIVE && (neighbors_count == 2 || neighbors_count == 3))) {
        new_CellStatus = ALIVE;
    }

    return new_CellStatus;
}

void update_cell(size_t index, const GameField *before, GameField *after) {
    after->data[index] = process_cell(index, before);
}

void process_range(const GameField *old_field, GameField *new_field, size_t start, size_t end) {
    for (size_t index = start; index < end; ++index) {
        new_field->data[index] = process_cell(index, old_field);
    }
}
