# Лаб 8 — Үндсэн санах ой (Бүлэг 9)
#   make        — addresses
#   make test   — номын жишээ, хил, буруу оролт; 9.6/9.7-ийн тооцоо
#   make clean
CC = gcc
CFLAGS = -Wall -Wextra -O2

all: addresses

addresses: addresses.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@for a in 19986 0 4095 4096 4294967295; do ./addresses $$a | tr '\n' ' '; echo; done
	@for b in 4294967296 -5 abc; do ./addresses $$b 2>&1; done; true
	@python3 memcalc.py

clean:
	rm -f addresses
