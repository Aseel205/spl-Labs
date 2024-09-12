all: main

main: main.o start.o util.o
	gcc -m32 -std=c11 main.o util.o start.o -o main

start.o: start.s
	nasm -f elf32 start.s -o start.o
util.o: util.c
	gcc -m32 -Wall -ansi -c -std=c11 -nostdlib -fno-stack-protector util.c -o util.o
main.o: main.c
	gcc -m32 -Wall -ansi -c -std=c11 -nostdlib -fno-stack-protector main.c -o main.o

clean:
	rm -f *.o main