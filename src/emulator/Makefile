CC      = gcc
CFLAGS  = -Wall -Werror -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic

.SUFFIXES: .c .o .h

.PHONY: all clean

all: emulate

emulate: emulate.o emulator_processor.o decode_helpers.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
