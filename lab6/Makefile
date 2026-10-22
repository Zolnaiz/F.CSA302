# Лаб №6 — Programming Problems 6.33, 6.34
#   make         race, mutex, monitor гурван программыг хөрвүүлнэ
#   make test    гуравыг 3 удаа ажиллуулж үр дүнг харьцуулна
#   make clean   хөрвүүлсэн файлуудыг устгана
CC = gcc
CFLAGS = -Wall -Wextra -pthread
PROGS = race mutex monitor

all: $(PROGS)

$(PROGS): %: %.c test_harness.c resource_manager.h
	$(CC) $(CFLAGS) -o $@ $< test_harness.c

test: all
	@for p in $(PROGS); do for i in 1 2 3; do \
	  printf "%-8s run %d: " $$p $$i; ./$$p | grep RESULT | cut -d: -f2; done; done

clean:
	rm -f $(PROGS)
