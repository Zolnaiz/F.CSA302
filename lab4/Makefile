# Лаб №4 — Programming Problems 4.22–4.28 (C программууд; 4.26, 4.29 нь Java)
#   make         бүх C программыг хөрвүүлнэ
#   make clean   хөрвүүлсэн файлуудыг устгана
# -pthread: Pthreads сан, -fopenmp: OpenMP (зөвхөн monte_carlo_omp).
CC = gcc
CFLAGS = -Wall -Wextra -pthread
PROGS = stats primes monte_carlo monte_carlo_omp fib pid_manager_mt

all: $(PROGS)

monte_carlo_omp: CFLAGS += -fopenmp

clean:
	rm -f $(PROGS)
