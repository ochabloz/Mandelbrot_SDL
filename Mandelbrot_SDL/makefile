CC=gcc
CFLAGS=-std=c99 -Wall
LIBS=-lm -lpthread -lSDL
OUT=mandelbrot_seq

$(OUT): $(OUT).o gfx.o
	$(CC) $^ -o $@ $(LIBS)

$(OUT).o: $(OUT).c gfx.h
	$(CC) $(CFLAGS) $< -c

gfx.o: gfx.c gfx.h
	$(CC) $(CFLAGS) $< -c

clean:
	rm -f $(OUT) *.o
