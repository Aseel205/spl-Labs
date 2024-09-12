all: loader

loader: loader.o start.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

loader.o: loader.c
	gcc -m32 -c loader.c -o loader.o

start.o: start.s
	nasm -g -f elf32 -o start.o start.s	

clean:
	rm -f loader.o loader