CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf -lm

SRC = src/main.c src/physics.c src/render.c src/trails.c
OBJ = $(SRC:.c=.o)
BIN = bin/double_pendulum_tron

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(BIN)
