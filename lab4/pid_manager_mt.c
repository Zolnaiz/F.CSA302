/*
 * 4.28 — 3.20-ийн PID manager-ийг олон урсгалаар турших.
 * 100 урсгал тус бүр pid авч, санамсаргүй хугацаагаар унтаад, pid-ээ буцаана.
 * Bitmap-ийг хоёр урсгал зэрэг өөрчилж ижил pid авахаас сэргийлж mutex ашиглана.
 *
 *   gcc -Wall -pthread -o pid_manager_mt pid_manager_mt.c
 *   ./pid_manager_mt
 */
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MIN_PID 300
#define MAX_PID 5000
#define NUM_PIDS (MAX_PID - MIN_PID + 1)
#define BITS (sizeof(unsigned int) * CHAR_BIT)
#define NUM_THREADS 100

/* ---------- 3.20-ийн PID manager (bitmap) ---------- */
static unsigned int *pid_map = NULL;
static int last_pid = MIN_PID - 1;
static pthread_mutex_t map_lock = PTHREAD_MUTEX_INITIALIZER;

static int test_bit(int i)  { return (pid_map[i / BITS] >> (i % BITS)) & 1u; }
static void set_bit(int i)  { pid_map[i / BITS] |=  (1u << (i % BITS)); }
static void clear_bit(int i){ pid_map[i / BITS] &= ~(1u << (i % BITS)); }

int allocate_map(void)
{
    pid_map = calloc((NUM_PIDS + BITS - 1) / BITS, sizeof *pid_map);
    return pid_map ? 1 : -1;
}

int allocate_pid(void)
{
    int pid = -1;
    pthread_mutex_lock(&map_lock);
    for (int k = 1; k <= NUM_PIDS; k++) {
        int i = (last_pid - MIN_PID + k) % NUM_PIDS;
        if (!test_bit(i)) {
            set_bit(i);
            pid = last_pid = MIN_PID + i;
            break;
        }
    }
    pthread_mutex_unlock(&map_lock);
    return pid;
}

void release_pid(int pid)
{
    if (pid < MIN_PID || pid > MAX_PID)
        return;
    pthread_mutex_lock(&map_lock);
    clear_bit(pid - MIN_PID);
    pthread_mutex_unlock(&map_lock);
}

/* ---------- Мулти-урсгалт тест ---------- */
static int in_use[MAX_PID + 1];             /* шалгалт: нэг pid давхар олгогдох эсэх */
static int duplicates = 0, max_active = 0, active = 0;
static pthread_mutex_t stat_lock = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg)
{
    long id = (long)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(id * 2654435761u);

    int pid = allocate_pid();
    if (pid == -1) {
        printf("Thread %3ld: no pid available\n", id);
        return NULL;
    }

    pthread_mutex_lock(&stat_lock);
    if (in_use[pid]++) duplicates++;
    if (++active > max_active) max_active = active;
    pthread_mutex_unlock(&stat_lock);

    int secs = rand_r(&seed) % 3 + 1;       /* 1-3 секунд "ажиллана" */
    printf("Thread %3ld: got pid %d, sleeping %d s\n", id, pid, secs);
    sleep(secs);

    pthread_mutex_lock(&stat_lock);
    in_use[pid]--;
    active--;
    pthread_mutex_unlock(&stat_lock);

    release_pid(pid);
    return NULL;
}

int main(void)
{
    if (allocate_map() == -1) {
        fprintf(stderr, "allocate_map failed\n");
        return EXIT_FAILURE;
    }

    pthread_t tid[NUM_THREADS];
    for (long i = 0; i < NUM_THREADS; i++)
        if (pthread_create(&tid[i], NULL, worker, (void *)i) != 0) {
            fprintf(stderr, "Error: pthread_create failed\n");
            return EXIT_FAILURE;
        }
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(tid[i], NULL);

    printf("\n%d threads done. Max active pids: %d, duplicate pids: %d\n",
           NUM_THREADS, max_active, duplicates);
    free(pid_map);
    return duplicates ? EXIT_FAILURE : 0;
}
