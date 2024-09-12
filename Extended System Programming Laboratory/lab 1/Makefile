CC = gcc
CFLAGS = -Wall -Wextra -g

all: addresses base count-words menu

addresses: addresses.c
	$(CC) $(CFLAGS) -o addresses addresses.c

base: base.c
	$(CC) $(CFLAGS) -o base base.c

count-words: count-words.c
	$(CC) $(CFLAGS) -o count-words count-words.c

menu: menu.c
	$(CC) $(CFLAGS) -o menu menu.c

clean:
	rm -f addresses base count-words menu
