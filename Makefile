# $Id$

CC = gcc

all: process_memory process_cpu process_threads
	strip $^

process_memory: common.o process_memory.c

process_cpu: common.o process_cpu.c

process_threads: common.o process_threads.c

clean:
	-rm -f process_memory process_cpu process_threads common.o

.PHONY: clean
