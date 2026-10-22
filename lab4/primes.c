/*
 * 4.23 — Анхны тоонуудыг тусдаа урсгалаар хэвлэх.
 * Хэрэглэгчийн өгсөн N-ээс бага буюу тэнцүү бүх анхны тоог
 * Эратосфений шигшүүрээр олж хэвлэнэ.
 *
 *   gcc -Wall -pthread -o primes primes.c
 *   ./primes 50
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *print_primes(void *arg)
{
    long n = *(long *)arg;
    char *composite = calloc(n + 1, 1);     /* 1 бол анхны биш */
    if (composite == NULL) {
        perror("calloc");
        return NULL;
    }

    for (long i = 2; i * i <= n; i++)
        if (!composite[i])
            for (long j = i * i; j <= n; j += i)
                composite[j] = 1;

    int printed = 0;
    for (long i = 2; i <= n; i++)
        if (!composite[i])
            printf(printed++ ? " %ld" : "%ld", i);
    printf("\n(%d primes <= %ld)\n", printed, n);

    free(composite);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    errno = 0;
    long n = strtol(argv[1], &end, 10);
    if (errno != 0 || *argv[1] == '\0' || *end != '\0' || n < 2) {
        fprintf(stderr, "Error: enter an integer >= 2\n");
        return EXIT_FAILURE;
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, print_primes, &n) != 0) {
        fprintf(stderr, "Error: pthread_create failed\n");
        return EXIT_FAILURE;
    }
    pthread_join(tid, NULL);
    return 0;
}
