# ==========================
#   Makefile for Windows
#   (MinGW / MSYS2 / GitBash)
# ==========================

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Iinclude
LDFLAGS = -lws2_32 

SRC = $(wildcard src/*.c) welcomehome.c
OBJ = $(SRC:.c=.o)

BIN = welcomehome.exe

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	del /Q $(subst /,\,$(OBJ)) $(BIN) 2>nul || true

.PHONY: all clean run