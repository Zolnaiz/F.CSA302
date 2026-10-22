# Лаб №3 — Programming Problems 3.18–3.22
#   make         бүх программыг хөрвүүлнэ
#   make clean   хөрвүүлсэн файлуудыг устгана
# time_shm, collatz_shm нь POSIX дундын санах ой (shm_open) ашигладаг тул -lrt хэрэгтэй.
CC = gcc
CFLAGS = -Wall -Wextra
PROGS = zombie time_shm time_pipe pid_manager collatz collatz_shm

all: $(PROGS)

time_shm collatz_shm: LDLIBS = -lrt

clean:
	rm -f $(PROGS)
