/*
 * 4.27 — Фибоначчийн дараалал тусдаа урсгалаар.
 * Хүү урсгал дарааллыг дундын массивт бичнэ. Эцэг урсгал
 * pthread_join()-оор (4.4-р хэсэг) хүлээгээд дарааллыг хэвлэнэ.
 *
 *   gcc -Wall -pthread -o fib fib.c
 *   ./fib 15
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FIB 94                          /* fib(93) нь unsigned 64-бит-д багтана */

static unsigned long long fib[MAX_FIB];     /* урсгалуудын дундаа хэрэглэх өгөгдөл */
static int n;

static void *generate(void *arg)
{
    (void)arg;
    if (n > 0) fib[0] = 0;
    if (n > 1) fib[1] = 1;
    for (int i = 2; i < n; i++)
        fib[i] = fib[i - 1] + fib[i - 2];
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    errno = 0;
    long v = strtol(argv[1], &end, 10);
    if (errno != 0 || *argv[1] == '\0' || *end != '\0' || v < 1 || v > MAX_FIB) {
        fprintf(stderr, "Error: enter an integer from 1 to %d\n", MAX_FIB);
        return EXIT_FAILURE;
    }
    n = (int)v;

    pthread_t tid;
    if (pthread_create(&tid, NULL, generate, NULL) != 0) {
        fprintf(stderr, "Error: pthread_create failed\n");
        return EXIT_FAILURE;
    }
    pthread_join(tid, NULL);                /* хүү урсгал дуустал хүлээнэ */

    for (int i = 0; i < n; i++)
        printf(i ? ", %llu" : "%llu", fib[i]);
    printf("\n");
    return 0;
}
