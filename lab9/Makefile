# Лаб 9 — Виртуал санах ой (Бүлэг 10)
#   make        — page_replace
#   make test   — номын жишээ (FIFO 15, LRU 12, OPT 9), 10.9, санамсаргүй мөр; 10.4–10.9-ийн тооцоо
#   make clean
CC = gcc
CFLAGS = -Wall -Wextra -O2

all: page_replace

page_replace: page_replace.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@./page_replace 3 -r "7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1"
	@./page_replace 3 -r "7 2 3 1 2 5 3 4 6 7 7 1 0 5 4 6 2 3 0 1" | tail -3
	@./page_replace 4 20 7
	@./page_replace 0 2>&1; true
	@python3 vmcalc.py

clean:
	rm -f page_replace
