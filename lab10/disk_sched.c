/*
 * disk_sched.c — Лаб 10, Programming Problem 11.27
 *
 * 0..4999 цилиндртэй диск, санамсаргүй 1000 хүсэлт. FCFS, SCAN, C-SCAN-аар үйлчилж,
 * толгойн нийт шилжилтийг (цилиндрээр) хэвлэнэ. Толгойн эхний байрлал командын мөрөөс.
 *
 *   SCAN   — толгой эхлээд 0 руу явж (номын Fig. 11.7 шиг), 0 дээр эргээд дээш.
 *   C-SCAN — толгой дээш (4999 хүртэл) явж, 0 руу шууд буцаад дахин дээш. Буцах
 *            шилжилтийг (4999 → 0) нийт дүнд оруулна.
 *
 *   gcc -Wall -o disk_sched disk_sched.c
 *   ./disk_sched 2150                     # санамсаргүй 1000 хүсэлт
 *   ./disk_sched 2150 -s 7                # seed 7 — давтагдах үр дүн
 *   ./disk_sched 53 -c 200 -r "98 183 37 122 14 124 65 67"
 *                                         # номын жишээ (§11.2): FCFS 640, SCAN 236, C-SCAN 382
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_REQ 100000

static int cmp(const void *a, const void *b)
{
	return *(const int *)a - *(const int *)b;
}

static long fcfs(const int *req, int n, int head)
{
	long total = 0;
	for (int i = 0; i < n; i++) {
		total += labs((long)req[i] - head);
		head = req[i];
	}
	return total;
}

/* SCAN: доошоо 0 хүртэл (замдаа доорх хүсэлтүүд), дараа нь дээш хамгийн том хүсэлт хүртэл. */
static long scan(const int *sorted, int n, int head)
{
	long total = head;                       /* head → 0 */
	if (n > 0 && sorted[n - 1] > head)
		total += sorted[n - 1];              /* 0 → хамгийн том хүсэлт */
	return total;
}

/* C-SCAN: дээш (cyl-1) хүртэл, 0 руу буцаад, толгойноос доорх хамгийн том хүсэлт хүртэл. */
static long cscan(const int *sorted, int n, int head, int cyl)
{
	long total = (cyl - 1) - head;           /* head → дээд зах */
	int below = -1;                          /* head-ээс доорх хамгийн том хүсэлт */
	for (int i = 0; i < n; i++)
		if (sorted[i] < head)
			below = sorted[i];
	if (below >= 0)
		total += (cyl - 1) + below;          /* дээд зах → 0 (буцах) → below */
	return total;
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

static void usage(const char *prog)
{
	fprintf(stderr, "Usage: %s <head> [-n requests] [-s seed] [-c cylinders] [-r \"c1 c2 ...\"]\n", prog);
	exit(1);
}

int main(int argc, char *argv[])
{
	int cyl = 5000, n = 1000, opt;
	unsigned seed = (unsigned)time(NULL);
	const char *list = NULL;

	while ((opt = getopt(argc, argv, "n:s:c:r:")) != -1) {
		switch (opt) {
		case 'n': n = parse_int(optarg, 1, MAX_REQ, "requests"); break;
		case 's': seed = (unsigned)parse_int(optarg, 0, 1 << 30, "seed"); break;
		case 'c': cyl = parse_int(optarg, 2, 1 << 20, "cylinders"); break;
		case 'r': list = optarg; break;
		default: usage(argv[0]);
		}
	}
	if (optind != argc - 1)
		usage(argv[0]);
	int head = parse_int(argv[optind], 0, cyl - 1, "head");

	static int req[MAX_REQ], sorted[MAX_REQ];
	if (list) {
		char *buf = strdup(list);
		n = 0;
		for (char *tok = strtok(buf, " ,"); tok; tok = strtok(NULL, " ,")) {
			if (n == MAX_REQ) {
				fprintf(stderr, "Error: more than %d requests\n", MAX_REQ);
				return 1;
			}
			req[n++] = parse_int(tok, 0, cyl - 1, "cylinder");
		}
		free(buf);
		if (n == 0) {
			fprintf(stderr, "Error: empty request list\n");
			return 1;
		}
	} else {
		srand(seed);
		for (int i = 0; i < n; i++)
			req[i] = rand() % cyl;
	}
	memcpy(sorted, req, n * sizeof req[0]);
	qsort(sorted, n, sizeof sorted[0], cmp);

	printf("Cylinders 0..%d, %d requests, initial head %d\n", cyl - 1, n, head);
	printf("FCFS:   total head movement = %ld cylinders\n", fcfs(req, n, head));
	printf("SCAN:   total head movement = %ld cylinders\n", scan(sorted, n, head));
	printf("C-SCAN: total head movement = %ld cylinders\n", cscan(sorted, n, head, cyl));
	return 0;
}
