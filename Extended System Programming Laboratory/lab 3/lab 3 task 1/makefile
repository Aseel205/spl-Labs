# Makefile

# Default target
all: start

# Linking start
start: start.o util.o
	gcc -m32 util.o start.o -o start

# Assembling start.s
start.o: start.s
	nasm -f elf32 start.s -o start.o

# Compiling util.c
util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

# Clean target to remove generated files
clean:
	rm -f *.o start
