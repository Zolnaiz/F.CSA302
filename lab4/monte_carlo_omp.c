/*
 * 4.25 — π-г Monte Carlo аргаар OpenMP ашиглан зэрэгцээ тооцох.
 * Цэг үүсгэх давталтыг parallel region-д хувааж, reduction(+) ашиглана.
 * π-г зөвхөн нэг удаа, parallel region-ээс гадна тооцно.
 *
 *   gcc -Wall -fopenmp -o monte_carlo_omp monte_carlo_omp.c
 *   ./monte_carlo_omp 10000000
 */
#include <errno.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    errno = 0;
    long total = strtol(argv[1], &end, 10);
    if (errno != 0 || *argv[1] == '\0' || *end != '\0' || total <= 0) {
        fprintf(stderr, "Error: enter a positive integer\n");
        return EXIT_FAILURE;
    }

    long hits = 0;
    int threads = 0;
    double t0 = omp_get_wtime();

    #pragma omp parallel reduction(+:hits)
    {
        /* урсгал бүр өөрийн seed-тэй — rand_r нь thread-safe */
        unsigned int seed = (unsigned int)time(NULL) ^ (omp_get_thread_num() * 7919u);

        #pragma omp single
        threads = omp_get_num_threads();

        #pragma omp for
        for (long i = 0; i < total; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0)
                hits++;
        }
    }

    double pi = 4.0 * hits / total;         /* parallel region-ээс гадна */
    printf("Threads: %d, points: %ld, in circle: %ld\n", threads, total, hits);
    printf("Estimated pi = %.6f  (%.3f s)\n", pi, omp_get_wtime() - t0);
    return 0;
}
