TARGET = mpi_game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.cpp $(GAME_CORE_PATH)/utils.c main.cpp
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h

$(TARGET): $(HEADERS) $(SOURCE)
	mpic++ $(SOURCE) -o $(TARGET) -Wall

clean:
	rm -rf *.o $(TARGET)
