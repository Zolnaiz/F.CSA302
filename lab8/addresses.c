/*
 * addresses.c — Лаб 8, Programming Problem 9.28
 *
 * 32 бит виртуал хаяг, 4 KB (4096 = 2^12) хуудас. Аравтын хаягаас
 *   хуудасны дугаар = хаяг >> 12      (дээд 20 бит)
 *   офсет           = хаяг & 0xFFF    (доод 12 бит)
 *
 *   gcc -Wall -o addresses addresses.c
 *   ./addresses 19986
 *   The address 19986 contains:
 *   page number = 4
 *   offset = 3602
 */
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE_BITS 12                         /* 4 KB = 2^12 */
#define PAGE_SIZE (1u << PAGE_BITS)

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <virtual address (decimal, 0..4294967295)>\n", argv[0]);
		return 1;
	}
	const char *s = argv[1];
	if (*s < '0' || *s > '9') {              /* хасах тэмдэг, үсэг, хоосон → алдаа */
		fprintf(stderr, "Error: '%s' is not a non-negative decimal number\n", s);
		return 1;
	}
	char *end;
	errno = 0;
	unsigned long long v = strtoull(s, &end, 10);
	if (*end != '\0') {
		fprintf(stderr, "Error: '%s' is not a non-negative decimal number\n", s);
		return 1;
	}
	if (errno == ERANGE || v > UINT32_MAX) {
		fprintf(stderr, "Error: %s does not fit in a 32-bit address (max %" PRIu32 ")\n", s, UINT32_MAX);
		return 1;
	}

	uint32_t address = (uint32_t)v;
	uint32_t page = address >> PAGE_BITS;
	uint32_t offset = address & (PAGE_SIZE - 1);

	printf("The address %" PRIu32 " contains:\n", address);
	printf("page number = %" PRIu32 "\n", page);
	printf("offset = %" PRIu32 "\n", offset);
	return 0;
}
