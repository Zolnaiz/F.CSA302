/*
 * 4.22 — Статистик утгууд олон урсгалаар.
 * Гурван worker урсгал: дундаж, хамгийн их, хамгийн бага утгыг тооцно.
 * Үр дүн глобал хувьсагчид хадгалагдаж, эцэг урсгал join хийсний дараа хэвлэнэ.
 *
 *   gcc -Wall -pthread -o stats stats.c
 *   ./stats 90 81 78 95 79 72 85
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static int *nums;
static int count;

/* worker урсгалуудын тохируулах глобал утгууд */
static double average;
static int minimum, maximum;

static void *calc_average(void *arg)
{
    (void)arg;
    long long sum = 0;
    for (int i = 0; i < count; i++)
        sum += nums[i];
    average = (double)sum / count;
    return NULL;
}

static void *calc_minimum(void *arg)
{
    (void)arg;
    minimum = nums[0];
    for (int i = 1; i < count; i++)
        if (nums[i] < minimum)
            minimum = nums[i];
    return NULL;
}

static void *calc_maximum(void *arg)
{
    (void)arg;
    maximum = nums[0];
    for (int i = 1; i < count; i++)
        if (nums[i] > maximum)
            maximum = nums[i];
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num1> <num2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    count = argc - 1;
    nums = malloc(count * sizeof *nums);
    if (nums == NULL) { perror("malloc"); return EXIT_FAILURE; }

    for (int i = 0; i < count; i++) {
        char *end;
        errno = 0;
        long v = strtol(argv[i + 1], &end, 10);
        if (errno != 0 || *argv[i + 1] == '\0' || *end != '\0') {
            fprintf(stderr, "Error: '%s' is not an integer\n", argv[i + 1]);
            free(nums);
            return EXIT_FAILURE;
        }
        nums[i] = (int)v;
    }

    pthread_t tid[3];
    void *(*workers[3])(void *) = { calc_average, calc_minimum, calc_maximum };

    for (int i = 0; i < 3; i++)
        if (pthread_create(&tid[i], NULL, workers[i], NULL) != 0) {
            fprintf(stderr, "Error: pthread_create failed\n");
            return EXIT_FAILURE;
        }
    for (int i = 0; i < 3; i++)
        pthread_join(tid[i], NULL);     /* worker-үүд дуусахыг хүлээнэ */

    printf("The average value is %.2f\n", average);
    printf("The minimum value is %d\n", minimum);
    printf("The maximum value is %d\n", maximum);

    free(nums);
    return 0;
}
