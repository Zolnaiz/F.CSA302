/*
 * 3.18 — Zombie процесс үүсгэх.
 * Хүү процесс шууд дуусна, эцэг нь wait() дуудахгүй 20 секунд унтана.
 * Энэ хугацаанд хүү процесс "Z" (zombie) төлөвт үлдэнэ.
 *
 *   ./zombie &
 *   ps -l            # S баганад Z харагдана
 *   kill -9 <PPID>   # эцгийг устгаснаар zombie арилна
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {                     /* хүү: шууд дуусна */
        printf("Child  PID=%d exiting -> becomes zombie\n", getpid());
        exit(0);
    }

    /* эцэг: wait() дуудахгүй тул хүүгийн ПУБ (PCB) устгагдахгүй */
    printf("Parent PID=%d, child PID=%d. Sleeping 20 s (no wait)...\n",
           getpid(), pid);
    fflush(stdout);
    sleep(2);
    system("ps -l");                    /* zombie-г харуулах */
    sleep(18);
    return 0;
}
