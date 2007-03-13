# $Id$

CC = gcc

all: munin/process_memory munin/process_cpu munin/process_threads nagios/check_process_memory nagios/check_unix
	strip $^

munin/process_memory: common.o memory.o munin/process_memory.c

nagios/check_process_memory: common.o memory.o nagios/check_process_memory.c

munin/process_cpu: common.o cpu.o munin/process_cpu.c

munin/process_threads: common.o munin/process_threads.c

nagios/check_unix: nagios/check_unix.c

clean:
	-rm -f munin/process_memory munin/process_cpu munin/process_threads nagios/check_process_memory nagios/check_unix
	-rm -f common.o memory.o cpu.o

.PHONY: clean
