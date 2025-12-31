CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk4) -Wall -Wextra -g
LIBS = $(shell pkg-config --libs gtk4 sdl2 SDL2_mixer) 

SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = tictactoe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
