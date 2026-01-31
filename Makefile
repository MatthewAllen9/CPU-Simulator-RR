CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: rr

rr: rr.o
	$(CC) $(CFLAGS) -o rr rr.o

rr.o: rr.c
	$(CC) $(CFLAGS) -c rr.c -o rr.o

clean:
	rm -f rr.o rr

