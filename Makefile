# $Id$

CC = gcc

all: process_memory process_cpu process_threads check_process_memory
	strip $^

process_memory: common.o process_memory.c memory.o

check_process_memory: common.o check_process_memory.c memory.o

process_cpu: common.o process_cpu.c

process_threads: common.o process_threads.c

clean:
	-rm -f process_memory process_cpu process_threads check_process_memory common.o memory.o

.PHONY: clean
