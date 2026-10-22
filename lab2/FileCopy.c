/*
 * Лаб №2 — FileCopy (POSIX хувилбар)
 * OSC 10th ed., 2.3-р хэсэг: нэг файлын агуулгыг өөр файл руу хуулах.
 *
 * Хөрвүүлэх:  gcc -Wall -Wextra -o FileCopy FileCopy.c
 * Ажиллуулах: ./FileCopy
 * Trace:      strace ./FileCopy
 */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE 4096

/* Хэрэглэгчээс нэг мөр уншиж, төгсгөлийн '\n'-г арилгана. */
static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL)
        return -1;                          /* EOF эсвэл уншилтын алдаа */

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    else if (len == size - 1) {
        fprintf(stderr, "Error: file name is too long\n");
        return -1;
    }
    return (int)len;
}

/* write() бүх байтыг нэг дор бичихгүй байж болох тул давтан бичнэ. */
static int write_all(int fd, const char *buf, ssize_t n)
{
    while (n > 0) {
        ssize_t w = write(fd, buf, (size_t)n);
        if (w == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        buf += w;
        n -= w;
    }
    return 0;
}

int main(void)
{
    char src[PATH_MAX], dst[PATH_MAX], answer[16];
    struct stat st_src, st_dst;

    /* 1. Эх болон хүлээн авах файлын нэрийг асуух */
    if (read_line("Enter source file name: ", src, sizeof src) <= 0) {
        fprintf(stderr, "Error: source file name is empty\n");
        return EXIT_FAILURE;
    }
    if (read_line("Enter destination file name: ", dst, sizeof dst) <= 0) {
        fprintf(stderr, "Error: destination file name is empty\n");
        return EXIT_FAILURE;
    }

    /* 2. Эх файл байгаа эсэх, энгийн файл мөн эсэхийг шалгах */
    if (stat(src, &st_src) == -1) {
        fprintf(stderr, "Error: cannot access '%s': %s\n", src, strerror(errno));
        return EXIT_FAILURE;
    }
    if (!S_ISREG(st_src.st_mode)) {
        fprintf(stderr, "Error: '%s' is not a regular file\n", src);
        return EXIT_FAILURE;
    }

    /* 3. Хүлээн авах файл аль хэдийн байвал шалгах */
    if (stat(dst, &st_dst) == 0) {
        if (st_dst.st_dev == st_src.st_dev && st_dst.st_ino == st_src.st_ino) {
            fprintf(stderr, "Error: source and destination are the same file\n");
            return EXIT_FAILURE;
        }
        if (S_ISDIR(st_dst.st_mode)) {
            fprintf(stderr, "Error: '%s' is a directory\n", dst);
            return EXIT_FAILURE;
        }
        if (read_line("Destination exists. Overwrite? (y/n): ",
                      answer, sizeof answer) < 0 ||
            (answer[0] != 'y' && answer[0] != 'Y')) {
            printf("Copy cancelled.\n");
            return EXIT_SUCCESS;
        }
    }

    /* 4. Эх файлыг нээх */
    int in = open(src, O_RDONLY);
    if (in == -1) {
        fprintf(stderr, "Error: cannot open '%s': %s\n", src, strerror(errno));
        return EXIT_FAILURE;
    }

    /* 5. Хүлээн авах файлыг үүсгэх (эрхийг эх файлаас авна) */
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st_src.st_mode & 0777);
    if (out == -1) {
        fprintf(stderr, "Error: cannot create '%s': %s\n", dst, strerror(errno));
        close(in);
        return EXIT_FAILURE;
    }

    /* 6. Уншиж-бичих давталт */
    char buf[BUF_SIZE];
    ssize_t n;
    long long total = 0;
    int status = EXIT_SUCCESS;

    while ((n = read(in, buf, sizeof buf)) != 0) {
        if (n == -1) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "Error: read failed: %s\n", strerror(errno));
            status = EXIT_FAILURE;
            break;
        }
        if (write_all(out, buf, n) == -1) {
            fprintf(stderr, "Error: write failed: %s\n", strerror(errno));
            status = EXIT_FAILURE;
            break;
        }
        total += n;
    }

    /* 7. Файлуудыг хаах (close-ийн алдааг ч шалгана) */
    if (close(in) == -1) {
        fprintf(stderr, "Error: close source: %s\n", strerror(errno));
        status = EXIT_FAILURE;
    }
    if (close(out) == -1) {
        fprintf(stderr, "Error: close destination: %s\n", strerror(errno));
        status = EXIT_FAILURE;
    }

    if (status == EXIT_SUCCESS)
        printf("Copied %lld bytes from '%s' to '%s'.\n", total, src, dst);
    return status;
}
