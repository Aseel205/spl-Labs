# Makefile for compiling hexaPrint, bubblesort, and virus_detector programs

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11

# Targets
TARGETS = hexaPrint bubblesort virus_detector

# Default target
all: $(TARGETS)

# Compile hexaPrint
hexaPrint: hexaPrint.o
	$(CC) $(CFLAGS) -o hexaPrint hexaPrint.o

# Compile bubblesort
bubblesort: bubblesort.o
	$(CC) $(CFLAGS) -o bubblesort bubblesort.o

# Compile virus_detector
virus_detector: virus_detector.o
	$(CC) $(CFLAGS) -o virus_detector virus_detector.o

# Compile hexaPrint.o
hexaPrint.o: hexaPrint.c
	$(CC) $(CFLAGS) -c hexaPrint.c

# Compile bubblesort.o
bubblesort.o: bubblesort.c
	$(CC) $(CFLAGS) -c bubblesort.c

# Compile virus_detector.o
virus_detector.o: virus_detector.c
	$(CC) $(CFLAGS) -c virus_detector.c

# Clean target to remove generated files
clean:
	rm -f $(TARGETS) *.o

# Phony targets
.PHONY: all clean
