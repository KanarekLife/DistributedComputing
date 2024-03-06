CC = gcc
CFLAGS = -g -Wall

all: main

main: main.c hashmap.c hashmap.h
	$(CC) $(CFLAGS) -o main main.c hashmap.c hashmap.h

run: main
	./main

clean:
	rm -f main

check: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main
