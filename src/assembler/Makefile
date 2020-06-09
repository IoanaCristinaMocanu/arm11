CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic

.SUFFIXES: .c .o .h

.PHONY: all clean

all: assemble

assemble: assemble.o label_table.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
