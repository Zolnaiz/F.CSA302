/*
 * Лаб №1 — C хэл дээрх "Hello world".
 * printf() нь стандарт C сангийн функц бөгөөд дотроо write() систем дуудалт хийдэг.
 *
 * Windows (PowerShell):  gcc hello.c -o hello.exe ; .\hello.exe
 * Linux   (WSL/PuTTY):   gcc hello.c -o /tmp/hello && /tmp/hello
 */
#include <stdio.h>
int main() {
    printf("Hello world\n");
    return 0;
}
