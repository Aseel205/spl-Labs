CC = gcc
CFLAGS = -Wall -Wextra -g -m32

all: myshell looper

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o

myshell.o: myshell.c LineParser.h
	$(CC) $(CFLAGS) -c myshell.c

LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c

looper: looper.o
	$(CC) $(CFLAGS) -o looper looper.o

looper.o: looper.c
	$(CC) $(CFLAGS) -c looper.c

clean:
	rm -f myshell myshell.o LineParser.o looper looper.o
