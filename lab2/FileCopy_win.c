/*
 * Лаб №2 — FileCopy (Windows API хувилбар)
 * OSC 10th ed., 2.3-р хэсэг: нэг файлын агуулгыг өөр файл руу хуулах.
 *
 * Хөрвүүлэх:  gcc -Wall -Wextra -g -o FileCopy.exe FileCopy_win.c
 * Ажиллуулах: .\FileCopy.exe
 * Дибаг:      gdb .\FileCopy.exe  (break CreateFileA / ReadFile / WriteFile)
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 4096

/* GetLastError()-ийн кодыг уншигдахуйц мессеж болгон хэвлэнэ. */
static void print_error(const char *what, const char *name)
{
    DWORD err = GetLastError();
    char msg[512] = "";

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                   msg, sizeof msg, NULL);
    msg[strcspn(msg, "\r\n")] = '\0';
    fprintf(stderr, "Error: %s '%s': %s (code %lu)\n", what, name, msg, err);
}

/* Хэрэглэгчээс нэг мөр уншиж, төгсгөлийн '\n'-г арилгана. */
static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL)
        return -1;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    else if (len == size - 1) {
        fprintf(stderr, "Error: file name is too long\n");
        return -1;
    }
    return (int)len;
}

/* Хоёр handle нэг файлыг заасан эсэхийг (volume + file index) шалгана. */
static BOOL same_file(HANDLE a, HANDLE b)
{
    BY_HANDLE_FILE_INFORMATION ia, ib;

    if (!GetFileInformationByHandle(a, &ia) || !GetFileInformationByHandle(b, &ib))
        return FALSE;
    return ia.dwVolumeSerialNumber == ib.dwVolumeSerialNumber &&
           ia.nFileIndexHigh == ib.nFileIndexHigh &&
           ia.nFileIndexLow == ib.nFileIndexLow;
}

int main(void)
{
    char src[MAX_PATH], dst[MAX_PATH], answer[16];

    /* 1. Эх болон хүлээн авах файлын нэрийг асуух */
    if (read_line("Enter source file name: ", src, sizeof src) <= 0) {
        fprintf(stderr, "Error: source file name is empty\n");
        return 1;
    }
    if (read_line("Enter destination file name: ", dst, sizeof dst) <= 0) {
        fprintf(stderr, "Error: destination file name is empty\n");
        return 1;
    }

    /* 2. Эх файл байгаа эсэх, хавтас биш эсэхийг шалгах */
    DWORD attr = GetFileAttributesA(src);
    if (attr == INVALID_FILE_ATTRIBUTES) {
        print_error("cannot access", src);
        return 1;
    }
    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        fprintf(stderr, "Error: '%s' is a directory\n", src);
        return 1;
    }

    /* 3. Эх файлыг нээх */
    HANDLE in = CreateFileA(src, GENERIC_READ, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (in == INVALID_HANDLE_VALUE) {
        print_error("cannot open", src);
        return 1;
    }

    /* 4. Хүлээн авах файл аль хэдийн байвал шалгах */
    DWORD dattr = GetFileAttributesA(dst);
    if (dattr != INVALID_FILE_ATTRIBUTES) {
        if (dattr & FILE_ATTRIBUTE_DIRECTORY) {
            fprintf(stderr, "Error: '%s' is a directory\n", dst);
            CloseHandle(in);
            return 1;
        }
        HANDLE probe = CreateFileA(dst, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (probe != INVALID_HANDLE_VALUE) {
            BOOL same = same_file(in, probe);
            CloseHandle(probe);
            if (same) {
                fprintf(stderr, "Error: source and destination are the same file\n");
                CloseHandle(in);
                return 1;
            }
        }
        if (read_line("Destination exists. Overwrite? (y/n): ",
                      answer, sizeof answer) < 0 ||
            (answer[0] != 'y' && answer[0] != 'Y')) {
            printf("Copy cancelled.\n");
            CloseHandle(in);
            return 0;
        }
    }

    /* 5. Хүлээн авах файлыг үүсгэх */
    HANDLE out = CreateFileA(dst, GENERIC_WRITE, 0, NULL,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (out == INVALID_HANDLE_VALUE) {
        print_error("cannot create", dst);
        CloseHandle(in);
        return 1;
    }

    /* 6. Уншиж-бичих давталт */
    char buf[BUF_SIZE];
    DWORD nread, nwritten;
    unsigned long long total = 0;
    int status = 0;

    for (;;) {
        if (!ReadFile(in, buf, sizeof buf, &nread, NULL)) {
            print_error("read failed on", src);
            status = 1;
            break;
        }
        if (nread == 0)                     /* файлын төгсгөл */
            break;
        if (!WriteFile(out, buf, nread, &nwritten, NULL) || nwritten != nread) {
            print_error("write failed on", dst);
            status = 1;
            break;
        }
        total += nread;
    }

    /* 7. Handle-уудыг хаах */
    if (!CloseHandle(in)) {
        print_error("close failed on", src);
        status = 1;
    }
    if (!CloseHandle(out)) {
        print_error("close failed on", dst);
        status = 1;
    }

    if (status == 0)
        printf("Copied %llu bytes from '%s' to '%s'.\n", total, src, dst);
    return status;
}
