TARGET = game
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.c $(GAME_CORE_PATH)/utils.c main.c barrier.c basic.c assistance.c assist_all.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h barrier.h basic.h assistance.h assist_all.h

$(TARGET): $(HEADERS) $(SOURCE)
	gcc-6 -std=c99 $(SOURCE) -o $(TARGET) -lpthread

clean:
	rm -rf *.o *.out $(TARGET)
