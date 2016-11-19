TARGET = game_sock
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.c $(GAME_CORE_PATH)/utils.c master.c slave.c main.c socket_utils.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h master.h slave.h socket_utils.h

$(TARGET): $(HEADERS) $(SOURCE)
	gcc-6 -std=c99 $(SOURCE) -o $(TARGET) -Wall -D_BSD_SOURCE

clean:
	rm -rf *.o $(TARGET)
