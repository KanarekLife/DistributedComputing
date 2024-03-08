CC = gcc
CFLAGS = -g -Wall

static:
	$(CC) -c hashmap.c -o hashmap.o $(CFLAGS)
	ar r libhashmap.a hashmap.o
	$(CC) -c main.c -o main.o $(CFLAGS)
	$(CC) main.o libhashmap.a -o main
	rm libhashmap.a hashmap.o main.o

run_static: static
	./main

dynamic:
	$(CC) -c $(CFLAGS) -fPIC -D_GNU_SOURCE hashmap.c -o hashmap.o
	$(CC) -shared -o libhashmap.so hashmap.o
	$(CC) -c main.c -o main.o $(CFLAGS)
	$(CC) main.o -L. -lhashmap -o main
	rm hashmap.o main.o

run_dynamic: dynamic
	LD_LIBRARY_PATH="$$LD_LIBRARY_PATH:." ./main

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f main

check: 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main
