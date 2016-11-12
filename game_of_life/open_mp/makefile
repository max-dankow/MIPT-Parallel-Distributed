TARGET = omp_game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.c $(GAME_CORE_PATH)/utils.c main.cpp
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h

$(TARGET): $(HEADERS) $(SOURCE)
	g++-6 -fopenmp $(SOURCE) -o $(TARGET) -Wall

clean:
	rm -rf *.o $(TARGET)
