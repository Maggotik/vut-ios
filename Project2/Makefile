CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt
LDFLAGS=-pthread -lrt

all: proj2

pro2: proj2.o
	$(CC) $(CFLAGS) $^ -o $@
proj2.o: proj2.c
	$(CC) $(CFLAGS) -c $<
clean: 
	rm -f *.o