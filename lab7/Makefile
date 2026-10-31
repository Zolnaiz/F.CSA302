# Лаб 7 — Түгжрэл (Бүлэг 8)
#   make        — bridge (C, pthreads) ба Transaction.class (Java)
#   make test   — гүүр (basic, fair), Java (ordered дуусна, naive гацна), банкирын алгоритм
#   make clean
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread

all: bridge Transaction.class

bridge: bridge.c
	$(CC) $(CFLAGS) -o $@ $<

Transaction.class: Transaction.java
	javac $<

test: all
	@echo "== 8.32 гүүр (basic)";  ./bridge 3 3 | tail -1
	@echo "== 8.31 гүүр (fair)";   ./bridge 4 4 fair | tail -1
	@echo "== 8.33 ordered";       java Transaction ordered | tail -1
	@echo "== 8.33 naive (deadlock хүлээгдэнэ)"; java Transaction naive | tail -2
	@echo "== банкирын алгоритм (8.3, 8.9)"; python3 banker.py | grep -E "АЮУЛГҮЙ|8.3 c:"

clean:
	rm -f bridge *.class
