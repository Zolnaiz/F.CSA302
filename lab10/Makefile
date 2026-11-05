# Лаб 10 — Их хэмжээний санах төхөөрөмж (Бүлэг 11)
#   make        — disk_sched
#   make test   — номын жишээ (FCFS 640, SCAN 236, C-SCAN 382), санамсаргүй 1000 хүсэлт; 11.7-ийн тооцоо
#   make clean
CC = gcc
CFLAGS = -Wall -Wextra -O2

all: disk_sched

disk_sched: disk_sched.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@./disk_sched 53 -c 200 -r "98 183 37 122 14 124 65 67"
	@./disk_sched 2150 -s 7
	@./disk_sched 5000 2>&1; true
	@python3 diskcalc.py

clean:
	rm -f disk_sched
