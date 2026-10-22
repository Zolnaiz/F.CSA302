/*
 * 3.22 — Collatz дараалал POSIX дундын санах ойгоор (shared memory).
 * Хүү процесс дарааллыг дундын санах ойд бичнэ, эцэг нь wait()-ийн дараа хэвлэнэ.
 *
 *   gcc -Wall -o collatz_shm collatz_shm.c -lrt
 *   ./collatz_shm 35
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_NAME "/lab3_collatz"
#define MAX_TERMS 4096

struct shared {
    int  count;                     /* дарааллын гишүүдийн тоо */
    long terms[MAX_TERMS];
};

static long parse_positive(const char *s)
{
    char *end;
    errno = 0;
    long n = strtol(s, &end, 10);
    if (errno != 0 || *s == '\0' || *end != '\0' || n <= 0)
        return -1;
    return n;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
        return EXIT_FAILURE;
    }
    long n = parse_positive(argv[1]);
    if (n == -1) {
        fprintf(stderr, "Error: '%s' is not a positive integer\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* a. Дундын санах ойн объект үүсгэх: shm_open, ftruncate, mmap */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return EXIT_FAILURE; }
    if (ftruncate(fd, sizeof(struct shared)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    struct shared *shm = mmap(NULL, sizeof *shm, PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    close(fd);

    /* b. Хүү процесс үүсгэж, дуусахыг хүлээх */
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    if (pid == 0) {                         /* хүү: дундын санах ойд бичнэ */
        shm->count = 0;
        shm->terms[shm->count++] = n;
        while (n != 1 && shm->count < MAX_TERMS) {
            n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
            shm->terms[shm->count++] = n;
        }
        _exit(0);
    }
    wait(NULL);

    /* c. Дундын санах ойн агуулгыг эцэг хэвлэнэ */
    for (int i = 0; i < shm->count; i++)
        printf(i ? ", %ld" : "%ld", shm->terms[i]);
    printf("\n(printed by parent from shared memory, %d terms)\n", shm->count);

    /* d. Дундын санах ойн объектыг устгах */
    munmap(shm, sizeof *shm);
    shm_unlink(SHM_NAME);
    return 0;
}
