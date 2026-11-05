/*
 * page_replace.c — Лаб 9, Programming Problem 10.44
 *
 * FIFO, LRU, OPT хуудас солих алгоритмыг нэг хуудасны лавлагааны мөр (page-reference
 * string) дээр ажиллуулж, тус бүрийн page fault-ийг тоолно. Хуудас 0..9, фрэймийн тоо
 * командын мөрөөс.
 *
 *   gcc -Wall -o page_replace page_replace.c
 *   ./page_replace 3                 # санамсаргүй 20 урттай мөр
 *   ./page_replace 3 30 7            # урт 30, seed 7 (давтагдах үр дүн)
 *   ./page_replace 3 -r "7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1"
 *                                    # номын жишээ (§10.4): FIFO 15, LRU 12, OPT 9
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FRAMES 64
#define MAX_LEN 100000
#define PAGES 10                             /* хуудасны дугаар 0..9 */

static int find(const int *frame, int used, int page)
{
	for (int i = 0; i < used; i++)
		if (frame[i] == page)
			return i;
	return -1;
}

/* FIFO: хамгийн эрт орсон хуудсыг гаргана — next нь тойрог дарааллын толгой. */
static int fifo(const int *ref, int n, int frames)
{
	int frame[MAX_FRAMES], used = 0, next = 0, faults = 0;
	for (int i = 0; i < n; i++) {
		if (find(frame, used, ref[i]) >= 0)
			continue;
		faults++;
		if (used < frames) {
			frame[used++] = ref[i];
		} else {
			frame[next] = ref[i];
			next = (next + 1) % frames;
		}
	}
	return faults;
}

/* LRU: хамгийн удаан хэрэглэгдээгүй хуудсыг гаргана — фрэйм бүрийн сүүлд хандсан хугацаа. */
static int lru(const int *ref, int n, int frames)
{
	int frame[MAX_FRAMES], last[MAX_FRAMES], used = 0, faults = 0;
	for (int i = 0; i < n; i++) {
		int k = find(frame, used, ref[i]);
		if (k < 0) {
			faults++;
			if (used < frames) {
				k = used++;
			} else {
				k = 0;
				for (int j = 1; j < used; j++)
					if (last[j] < last[k])
						k = j;
			}
			frame[k] = ref[i];
		}
		last[k] = i;
	}
	return faults;
}

/* OPT: ирээдүйд хамгийн сүүлд (эсвэл хэзээ ч) хэрэглэгдэх хуудсыг гаргана. */
static int opt(const int *ref, int n, int frames)
{
	int frame[MAX_FRAMES], used = 0, faults = 0;
	for (int i = 0; i < n; i++) {
		if (find(frame, used, ref[i]) >= 0)
			continue;
		faults++;
		if (used < frames) {
			frame[used++] = ref[i];
			continue;
		}
		int victim = 0, farthest = -1;
		for (int j = 0; j < used; j++) {
			int t = i + 1;
			while (t < n && ref[t] != frame[j])
				t++;
			if (t > farthest) {          /* t == n: дахин хэрэглэгдэхгүй */
				farthest = t;
				victim = j;
			}
		}
		frame[victim] = ref[i];
	}
	return faults;
}

static int parse_int(const char *s, int lo, int hi, const char *what)
{
	char *end;
	long v = strtol(s, &end, 10);
	if (*s == '\0' || *end != '\0' || v < lo || v > hi) {
		fprintf(stderr, "Error: %s must be an integer in %d..%d (got '%s')\n", what, lo, hi, s);
		exit(1);
	}
	return (int)v;
}

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 4) {
		fprintf(stderr, "Usage: %s <frames> [length [seed]]\n"
				"       %s <frames> -r \"<page numbers 0..9>\"\n", argv[0], argv[0]);
		return 1;
	}
	int frames = parse_int(argv[1], 1, MAX_FRAMES, "frames");
	static int ref[MAX_LEN];
	int n = 0;

	if (argc >= 3 && strcmp(argv[2], "-r") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Error: -r needs a reference string\n");
			return 1;
		}
		char *buf = strdup(argv[3]);
		for (char *tok = strtok(buf, " ,"); tok; tok = strtok(NULL, " ,")) {
			if (n == MAX_LEN) {
				fprintf(stderr, "Error: reference string longer than %d\n", MAX_LEN);
				return 1;
			}
			ref[n++] = parse_int(tok, 0, PAGES - 1, "page number");
		}
		free(buf);
		if (n == 0) {
			fprintf(stderr, "Error: empty reference string\n");
			return 1;
		}
	} else {
		n = argc >= 3 ? parse_int(argv[2], 1, MAX_LEN, "length") : 20;
		unsigned seed = argc == 4 ? (unsigned)parse_int(argv[3], 0, 1 << 30, "seed")
					  : (unsigned)time(NULL);
		srand(seed);
		for (int i = 0; i < n; i++)
			ref[i] = rand() % PAGES;
	}

	printf("Page frames: %d\n", frames);
	printf("Reference string (%d):", n);
	for (int i = 0; i < n && i < 40; i++)
		printf(" %d", ref[i]);
	printf(n > 40 ? " ...\n" : "\n");
	printf("FIFO: %d page faults\n", fifo(ref, n, frames));
	printf("LRU:  %d page faults\n", lru(ref, n, frames));
	printf("OPT:  %d page faults\n", opt(ref, n, frames));
	return 0;
}
