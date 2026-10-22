/*
 * 3.19 (1-р хувилбар) — Командын ажиллах хугацааг хэмжих, IPC: POSIX дундын санах ой (shared memory).
 * Хүү процесс exec()-ийн өмнө эхлэх хугацааг дундын санах ойд бичнэ,
 * эцэг нь хүү дууссаны дараа уншиж зөрүүг тооцно.
 *
 *   gcc -Wall -o time_shm time_shm.c -lrt
 *   ./time_shm ls -l
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_NAME "/lab3_time_shm"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* fork()-оос өмнө дундын санах ой үүсгэнэ — хоёулаа хандана */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return EXIT_FAILURE; }
    if (ftruncate(fd, sizeof(struct timeval)) == -1) {
        perror("ftruncate");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    struct timeval *start = mmap(NULL, sizeof *start, PROT_READ | PROT_WRITE,
                                 MAP_SHARED, fd, 0);
    if (start == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    close(fd);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shm_unlink(SHM_NAME);
        return EXIT_FAILURE;
    }
    if (pid == 0) {                         /* хүү процесс */
        gettimeofday(start, NULL);          /* эхлэх хугацаа -> дундын санах ой */
        execvp(argv[1], &argv[1]);
        perror("execvp");                   /* exec амжилтгүй бол л энд ирнэ */
        _exit(127);
    }

    int status;
    waitpid(pid, &status, 0);               /* эцэг: хүүгээ хүлээнэ */
    struct timeval end;
    gettimeofday(&end, NULL);

    double elapsed = (end.tv_sec - start->tv_sec) +
                     (end.tv_usec - start->tv_usec) / 1e6;
    printf("Elapsed time: %.5f\n", elapsed);

    munmap(start, sizeof *start);
    shm_unlink(SHM_NAME);
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
