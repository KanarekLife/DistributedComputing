CC = gcc
CFLAGS = -Wall -Wextra

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c -lpng -lpthread -lm

run: main
	./main

clean:
	rm -f main