CC=gcc
LIB=-lrt -lpthread -lm -lSDL
CCFLAGS=-std=c99

all: Mandelbrot

Mandelbrot: spinlock.o gfx.o main.o stack.o Mandelbrot.o
	$(CC) $(CCFLAGS) -o Mandelbrot gfx.o main.o spinlock.o stack.o Mandelbrot.o $(LIB)

gfx.o: gfx.c gfx.h
	$(CC) $(CCFLAGS) -c gfx.c $(LIB)

main.o: main.c
	$(CC) $(CCFLAGS) -c main.c $(LIB)

spinlock.o: spinlock.c spinlock.h
	$(CC) $(CCFLAGS) -c spinlock.c $(LIB)

stack.o: stack.c stack.h
	$(CC) $(CCFLAGS) -c stack.c $(LIB)

Mandelbrot.o: Mandelbrot.c Mandelbrot.h
	$(CC) $(CCFLAGS) -c Mandelbrot.c $(LIB)

clean:
	rm -rf *.o
	rm -rf Mandelbrot

