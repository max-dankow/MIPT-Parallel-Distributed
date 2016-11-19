TARGET = omp_game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.c $(GAME_CORE_PATH)/utils.c main.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h

$(TARGET): $(HEADERS) $(SOURCE)
	gcc-6 -std=c99 -fopenmp $(SOURCE) -o $(TARGET) -Wall

clean:
	rm -rf *.o *.out $(TARGET)
