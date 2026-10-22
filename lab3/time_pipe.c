/*
 * 3.19 (2-р хувилбар) — Командын ажиллах хугацааг хэмжих, IPC: ordinary pipe.
 * Хүү процесс exec()-ийн өмнө эхлэх хугацааг pipe руу бичнэ,
 * эцэг нь хүү дууссаны дараа pipe-аас уншина.
 *
 *   gcc -Wall -o time_pipe time_pipe.c
 *   ./time_pipe ls -l
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd[2];                              /* fd[0] — унших, fd[1] — бичих */
    if (pipe(fd) == -1) { perror("pipe"); return EXIT_FAILURE; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return EXIT_FAILURE; }

    if (pid == 0) {                         /* хүү процесс */
        close(fd[0]);
        struct timeval start;
        gettimeofday(&start, NULL);
        if (write(fd[1], &start, sizeof start) != sizeof start)
            perror("write");
        close(fd[1]);
        execvp(argv[1], &argv[1]);
        perror("execvp");
        _exit(127);
    }

    close(fd[1]);                           /* эцэг: зөвхөн уншина */
    int status;
    waitpid(pid, &status, 0);
    struct timeval end, start;
    gettimeofday(&end, NULL);

    if (read(fd[0], &start, sizeof start) != sizeof start) {
        fprintf(stderr, "Error: could not read start time from pipe\n");
        return EXIT_FAILURE;
    }
    close(fd[0]);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1e6;
    printf("Elapsed time: %.5f\n", elapsed);
    return WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
}
