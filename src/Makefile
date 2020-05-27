CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic

.SUFFIXES: .c .o .h

.PHONY: all clean

all: assemble emulate

assemble: assemble.o

emulate: emulate.o emulator_processor.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
