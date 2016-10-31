TARGET = game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.cpp $(GAME_CORE_PATH)/utils.c main.c barrier.c basic.c assistance.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h barrier.h basic.h assistance.h

$(TARGET): $(HEADERS) $(SOURCE)
	g++ $(SOURCE) -o $(TARGET) -lpthread

clean:
	rm -rf *.o $(TARGET)
