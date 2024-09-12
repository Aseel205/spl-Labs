CC = gcc
CFLAGS = -Wall -g

# List of object files
OBJS = myshell.o mypipeline.o

all: myshell mypipeline

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o

mypipeline: mypipeline.o LineParser.o
	$(CC) $(CFLAGS) -o mypipeline mypipeline.o LineParser.o

# Compile each source file into object files
myshell.o: myshell.c LineParser.h
	$(CC) $(CFLAGS) -c myshell.c

mypipeline.o: mypipeline.c LineParser.h
	$(CC) $(CFLAGS) -c mypipeline.c

LineParser.o: LineParser.c LineParser.h
	$(CC) $(CFLAGS) -c LineParser.c

# Clean up object files and executables
clean:
	rm -f *.o myshell mypipeline
