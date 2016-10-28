TARGET = game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.cpp main.c barrier.c basic.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h barrier.h basic.h

$(TARGET): $(HEADERS) $(SOURCE)
	g++ -std=c++11 $(SOURCE) -o $(TARGET) -Wall

clean:
	rm -rf *.o $(TARGET)
