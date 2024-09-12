CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all clean

all: encoder

encoder: encoder.o
	$(CC) $(CFLAGS) -o $@ $^

encoder.o: encoder.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f encoder *.o
