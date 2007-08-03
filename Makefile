# $Id$

CC = gcc

all: munin/process_memory munin/process_cpu \
	munin/process_threads nagios/check_process_memory \
	nagios/check_process_memory nagios/check_unix \
	nagios/check_process_cpu utils/setuidgid \
	utils/zoperestart

munin/process_memory: common.o memory.o munin/process_memory.o
	$(CC) $^ -o $@
	strip $@

nagios/check_process_memory: common.o memory.o nagios/check_process_memory.o
	$(CC) $^ -o $@
	strip $@

nagios/check_process_cpu: common.o cpu.o nagios/check_process_cpu.o
	$(CC) $^ -o $@
	strip $@

munin/process_cpu: common.o cpu.o munin/process_cpu.o
	$(CC) $^ -o $@
	strip $@

munin/process_threads: common.o munin/process_threads.o
	$(CC) $^ -o $@
	strip $@

nagios/check_unix: nagios/check_unix.o
	$(CC) $^ -o $@
	strip $@

utils/setuidgid: utils/setuidgid.o
	$(CC) $^ -o $@
	strip $@

utils/zoperestart: utils/zoperestart.o
	$(CC) $^ -o $@
	strip $@

clean:
	-rm -f munin/process_memory munin/process_cpu munin/process_threads
	-rm -f nagios/check_process_memory nagios/check_unix nagios/check_process_cpu
	-rm -f utils/setuidgid utils/zoperestart
	-rm -f *.o
	-rm -f munin/*.o
	-rm -f nagios/*.o
	-rm -f utils/*.o

.PHONY: clean all
