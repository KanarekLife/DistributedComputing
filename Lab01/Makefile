CC = gcc
CFLAGS = -Wall -Wextra

all: main

main: main.c hashmap.c hashmap.h
	$(CC) $(CFLAGS) -o main main.c hashmap.c hashmap.h

run: main
	./main

clean:
	rm -f main

check: main
	ulimit -n 1024
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./main