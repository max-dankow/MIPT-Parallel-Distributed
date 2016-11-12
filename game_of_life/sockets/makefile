TARGET = game_sock
GAME_CORE_PATH = ../core
SOURCE = $(GAME_CORE_PATH)/game.c $(GAME_CORE_PATH)/utils.c master.c slave.c main.c
HEADERS = $(GAME_CORE_PATH)/game.h $(GAME_CORE_PATH)/utils.h master.h slave.h

$(TARGET): $(HEADERS) $(SOURCE)
	gcc-6 $(SOURCE) -o $(TARGET) -Wall

clean:
	rm -rf *.o $(TARGET)
