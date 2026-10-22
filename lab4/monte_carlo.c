/*
 * 4.24 — π-г Monte Carlo аргаар тусдаа урсгал ашиглан тооцох.
 * Урсгал [-1,1]x[-1,1] квадратад санамсаргүй цэг үүсгэж, тойрог дотор
 * орсон цэгийн тоог глобал хувьсагчид хадгална. Эцэг урсгал π-г тооцно.
 *
 *   gcc -Wall -pthread -o monte_carlo monte_carlo.c
 *   ./monte_carlo 10000000
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static long total_points;
static long circle_points;                  /* урсгалын тохируулах глобал утга */

/* [-1, 1] завсарт санамсаргүй тоо */
static double random_double(unsigned int *seed)
{
    return (double)rand_r(seed) / RAND_MAX * 2.0 - 1.0;
}

static void *generate_points(void *arg)
{
    (void)arg;
    unsigned int seed = (unsigned int)time(NULL);
    long hits = 0;

    for (long i = 0; i < total_points; i++) {
        double x = random_double(&seed);
        double y = random_double(&seed);
        if (x * x + y * y <= 1.0)           /* тойрог дотор */
            hits++;
    }
    circle_points = hits;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *end;
    errno = 0;
    total_points = strtol(argv[1], &end, 10);
    if (errno != 0 || *argv[1] == '\0' || *end != '\0' || total_points <= 0) {
        fprintf(stderr, "Error: enter a positive integer\n");
        return EXIT_FAILURE;
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, generate_points, NULL) != 0) {
        fprintf(stderr, "Error: pthread_create failed\n");
        return EXIT_FAILURE;
    }
    pthread_join(tid, NULL);

    double pi = 4.0 * circle_points / total_points;
    printf("Points: %ld, in circle: %ld\n", total_points, circle_points);
    printf("Estimated pi = %.6f\n", pi);
    return 0;
}
