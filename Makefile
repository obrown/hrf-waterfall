CC=gcc

LDFLAGS=-lhackrf -lfftw3 -lm -lSDL2
OPTFLAGS=-O3 -march=native -fopenmp
WARNFLAGS=-Wall -Wextra

CFLAGS=-std=c99 -fopenmp $(OPTFLAGS) $(LDFLAGS) $(WARNFLAGS)

SOURCES=main.c colormap.c queue.c waterfall.c device.c fft.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f main
