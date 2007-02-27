# $Id$

CC = gcc

all: process_memory process_cpu
	strip $^

process_memory: common.o process_memory.c

process_cpu: common.o process_cpu.c

clean:
	-rm -f process_memory process_cpu

.PHONY: clean
