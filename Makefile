#Following blocks of code were made with the assistance of Co-pilot generated code.

CC=gcc
CFLAGS=-Wall -Werror -Wextra
SRC=ghost.c house.c hunter.c logger.c main.c room.c utils.c
OBJ=$(SRC:.c=.o)
EXECUTABLE=game
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

.PHONY: all clean valgrind

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c defs.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(EXECUTABLE)

valgrind: $(EXECUTABLE)
	$(VALGRIND) ./$(EXECUTABLE)