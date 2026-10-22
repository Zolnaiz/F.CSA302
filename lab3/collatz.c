/*
 * 3.21 — Collatz дараалал fork()-оор: хүү процесс дарааллыг хэвлэнэ,
 * эцэг нь wait()-ээр хүлээнэ.
 *
 *   gcc -Wall -o collatz collatz.c
 *   ./collatz 35
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Командын мөрийн аргументыг эерэг бүхэл тоо мөн эсэхийг шалгана. */
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

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (pid == 0) {                         /* хүү: дарааллыг хэвлэнэ */
        printf("%ld", n);
        while (n != 1) {
            n = (n % 2 == 0) ? n / 2 : 3 * n + 1;
            printf(", %ld", n);
        }
        printf("\n");
        return 0;
    }

    wait(NULL);                             /* эцэг: хүүгээ хүлээнэ */
    printf("Parent: child %d finished\n", pid);
    return 0;
}
