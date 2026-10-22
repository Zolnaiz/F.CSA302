/*
 * 3.20 — PID manager (bitmap).
 * Битийн i-р байрлал 0 бол (MIN_PID + i) pid чөлөөтэй, 1 бол ашиглагдаж байна.
 *
 *   gcc -Wall -o pid_manager pid_manager.c
 *   ./pid_manager
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_PID 300
#define MAX_PID 5000
#define NUM_PIDS (MAX_PID - MIN_PID + 1)
#define BITS (sizeof(unsigned int) * CHAR_BIT)

static unsigned int *pid_map = NULL;
static int last_pid = MIN_PID - 1;          /* дараагийн хайлтыг эндээс эхэлнэ */

/* Bitmap үүсгэж бүгдийг 0 (чөлөөтэй) болгоно. Амжилттай бол 1, үгүй бол -1. */
int allocate_map(void)
{
    size_t words = (NUM_PIDS + BITS - 1) / BITS;
    pid_map = calloc(words, sizeof *pid_map);
    return pid_map ? 1 : -1;
}

static int test_bit(int i)  { return (pid_map[i / BITS] >> (i % BITS)) & 1u; }
static void set_bit(int i)  { pid_map[i / BITS] |=  (1u << (i % BITS)); }
static void clear_bit(int i){ pid_map[i / BITS] &= ~(1u << (i % BITS)); }

/* Чөлөөтэй pid олж буцаана, бүгд ашиглагдаж байвал -1. */
int allocate_pid(void)
{
    if (pid_map == NULL)
        return -1;
    for (int k = 1; k <= NUM_PIDS; k++) {   /* last_pid-ээс хойш тойрч хайна */
        int i = (last_pid - MIN_PID + k) % NUM_PIDS;
        if (!test_bit(i)) {
            set_bit(i);
            last_pid = MIN_PID + i;
            return last_pid;
        }
    }
    return -1;
}

/* pid-г чөлөөлнө (хүрээнээс гадуурх утгыг үл тооно). */
void release_pid(int pid)
{
    if (pid_map && pid >= MIN_PID && pid <= MAX_PID)
        clear_bit(pid - MIN_PID);
}

int main(void)
{
    if (allocate_map() == -1) {
        fprintf(stderr, "allocate_map failed\n");
        return EXIT_FAILURE;
    }

    int a = allocate_pid(), b = allocate_pid(), c = allocate_pid();
    printf("Allocated: %d %d %d\n", a, b, c);

    release_pid(b);
    printf("Released %d, next allocation: %d\n", b, allocate_pid());

    int count = 3;                          /* a, c, шинэ нэг = 3 ашиглагдаж байна */
    while (allocate_pid() != -1)
        count++;
    printf("Allocated all %d pids (range %d-%d). Next: %d (expected -1)\n",
           count, MIN_PID, MAX_PID, allocate_pid());

    release_pid(4000);
    printf("Released 4000, next allocation: %d\n", allocate_pid());

    free(pid_map);
    return 0;
}
