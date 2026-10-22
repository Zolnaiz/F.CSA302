/*
 * Лаб №6 — Нөөц менежерийн тест (race.c / mutex.c / monitor.c-тэй холбож хөрвүүлнэ).
 *
 * N урсгал тус бүр ITER удаа: 1–3 нөөц авах → ашиглах (богино унтах) → буцааж өгөх.
 * Шалгах нөхцөл (invariant):
 *   1. Нэгэн зэрэг эзэмшиж буй нөөц хэзээ ч MAX_RESOURCES-ээс хэтрэхгүй.
 *   2. Бүгд дууссаны дараа available_resources == MAX_RESOURCES.
 * Тестийн өөрийн тоолуурууд (held, granted ...) атомар (stdatomic) тул тэдгээр нь
 * уралдааны нөхцөлгүй — зөвхөн шалгаж буй нөөц менежер л синхрончлолгүй байж болно.
 *
 *   ./race [урсгалын тоо] [давталт]      (анхдагч: 8 урсгал, 300 давталт)
 */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "resource_manager.h"

static int iterations = 300;
static atomic_int held, max_held, granted, denied;

static void *worker(void *arg)
{
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)((long)arg * 2654435761u);

    for (int i = 0; i < iterations; i++) {
        int count = rand_r(&seed) % 3 + 1;
        if (decrease_count(count) != 0) {
            atomic_fetch_add(&denied, 1);
            continue;
        }
        atomic_fetch_add(&granted, 1);

        int now = atomic_fetch_add(&held, count) + count;   /* одоо эзэмшиж буй нийт */
        int prev = atomic_load(&max_held);
        while (now > prev && !atomic_compare_exchange_weak(&max_held, &prev, now))
            ;

        usleep(rand_r(&seed) % 50);                          /* нөөцийг "ашиглана" */

        atomic_fetch_sub(&held, count);
        increase_count(count);
    }
    return NULL;
}

static int parse_arg(const char *s, int lo, int hi, const char *what)
{
    char *end;
    long v = strtol(s, &end, 10);
    if (*s == '\0' || *end != '\0' || v < lo || v > hi) {
        fprintf(stderr, "Error: %s must be an integer in [%d, %d]\n", what, lo, hi);
        exit(2);
    }
    return (int)v;
}

int main(int argc, char *argv[])
{
    int nthreads = 8;
    if (argc > 1) nthreads = parse_arg(argv[1], 1, 64, "threads");
    if (argc > 2) iterations = parse_arg(argv[2], 1, 100000, "iterations");

    printf("Implementation : %s\n", impl_name);
    printf("Config         : MAX_RESOURCES=%d, threads=%d, iterations=%d\n",
           MAX_RESOURCES, nthreads, iterations);

    /* захын утгын шалгалт: MAX-аас их хүсэлт шууд -1 байх ёстой */
    printf("Edge test      : decrease_count(%d) = %d (expect -1)\n",
           MAX_RESOURCES + 1, decrease_count(MAX_RESOURCES + 1));

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    pthread_t tid[64];
    for (long i = 0; i < nthreads; i++)
        if (pthread_create(&tid[i], NULL, worker, (void *)i) != 0) {
            fprintf(stderr, "Error: pthread_create failed\n");
            return 2;
        }
    for (int i = 0; i < nthreads; i++)
        pthread_join(tid[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    int avail = get_available();
    int peak = atomic_load(&max_held);
    int ok = (peak <= MAX_RESOURCES) && (avail == MAX_RESOURCES);

    printf("Granted/denied : %d / %d\n", atomic_load(&granted), atomic_load(&denied));
    printf("Peak held      : %d (must be <= %d)%s\n", peak, MAX_RESOURCES,
           peak > MAX_RESOURCES ? "   <-- OVER-ALLOCATION" : "");
    printf("Final available: %d (must be == %d)%s\n", avail, MAX_RESOURCES,
           avail != MAX_RESOURCES ? "   <-- LOST UPDATE" : "");
    printf("Time           : %.2f s\n", secs);
    printf("RESULT         : %s\n", ok ? "OK - no race detected" : "RACE CONDITION DETECTED");
    return ok ? 0 : 1;
}
