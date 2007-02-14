# $Id$

CC = gcc

all: process_memory
	strip process_memory

clean:
	-rm -f process_memory

.PHONY: clean
