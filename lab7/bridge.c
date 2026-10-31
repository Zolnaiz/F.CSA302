/*
 * bridge.c — Лаб 7, Programming Problem 8.32 (Exercise 8.30, 8.31)
 *
 * Нэг эгнээтэй гүүр: хойд ба өмнөд зүгийн тариаланчид (урсгал) гүүрээр гарна.
 * Эсрэг чиглэлийн хоёр тариаланч зэрэг гүүрэнд орвол deadlock — хэн нь ч ухрахгүй.
 * Шийдэл: мутекс + нөхцөлт хувьсагч. Гүүрэн дээр зөвхөн НЭГ чиглэлийнхэн байна
 * (нэг чиглэлийнхэн дараалан орж болно); эсрэг чиглэлийнх гүүр хоосрохыг хүлээнэ.
 *
 *   ./bridge N S          — 8.30: deadlock-гүй (starvation байж болно)
 *   ./bridge N S fair     — 8.31: starvation-гүй — нэг чиглэлээс дараалан
 *                           хамгийн ихдээ MAX_RUN хүн орсны дараа эсрэг талд ээлж өгнө
 *
 *   gcc -Wall -pthread -o bridge bridge.c && ./bridge 4 4 fair
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

enum { NORTH, SOUTH };
static const char *NAME[] = { "хойд", "өмнөд" };
#define MAX_RUN 2                   /* fair горимд дараалан орох дээд тоо */

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  changed = PTHREAD_COND_INITIALIZER;
static int on_bridge[2];            /* гүүрэн дээрх хүн (чиглэл тус бүр) */
static int waiting[2];              /* хүлээж буй хүн */
static int last_dir = -1;           /* хамгийн сүүлд орсон чиглэл */
static int run;                     /* last_dir-ээс дараалан орсон тоо */
static int fair;
static int violations;              /* хоёр чиглэл зэрэг гүүрэн дээр байсан тоо (0 байх ёстой) */
static struct timespec t0;

static double now(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (t.tv_sec - t0.tv_sec) + (t.tv_nsec - t0.tv_nsec) / 1e9;
}

static int may_enter(int dir)
{
	int other = 1 - dir;
	if (on_bridge[other] > 0)
		return 0;                           /* эсрэг чиглэлийнх гүүрэн дээр */
	if (fair && dir == last_dir && run >= MAX_RUN && waiting[other] > 0)
		return 0;                           /* ээлж эсрэг талд шилжинэ */
	return 1;
}

static void enter_bridge(int dir, int id)
{
	pthread_mutex_lock(&lock);
	waiting[dir]++;
	while (!may_enter(dir))
		pthread_cond_wait(&changed, &lock);
	waiting[dir]--;
	if (dir != last_dir) {                  /* чиглэл солигдлоо — шинэ ээлж */
		last_dir = dir;
		run = 0;
	}
	on_bridge[dir]++;
	run++;
	if (on_bridge[1 - dir] > 0)
		violations++;
	printf("%6.2f с  %s #%d гүүрэнд орлоо   (гүүрэн дээр: хойд %d, өмнөд %d)\n",
	       now(), NAME[dir], id, on_bridge[NORTH], on_bridge[SOUTH]);
	pthread_mutex_unlock(&lock);
}

static void exit_bridge(int dir, int id)
{
	pthread_mutex_lock(&lock);
	on_bridge[dir]--;
	printf("%6.2f с  %s #%d гүүрээс гарлаа\n", now(), NAME[dir], id);
	pthread_cond_broadcast(&changed);
	pthread_mutex_unlock(&lock);
}

struct farmer { int dir, id; unsigned seed; };

static void *farmer(void *arg)
{
	struct farmer *f = arg;
	usleep(rand_r(&f->seed) % 300000);              /* ирэх хугацаа санамсаргүй */
	enter_bridge(f->dir, f->id);
	usleep(200000 + rand_r(&f->seed) % 400000);     /* гүүрээр явах хугацаа */
	exit_bridge(f->dir, f->id);
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Хэрэглээ: %s <хойд_тоо> <өмнөд_тоо> [fair]\n", argv[0]);
		return 1;
	}
	int n = atoi(argv[1]), s = atoi(argv[2]);
	if (n < 0 || s < 0 || n + s == 0 || n + s > 100) {
		fprintf(stderr, "Тоо 0..100 хооронд, нийлбэр нь 1-ээс их байна\n");
		return 1;
	}
	fair = argc > 3 && strcmp(argv[3], "fair") == 0;
	clock_gettime(CLOCK_MONOTONIC, &t0);
	printf("Гүүр: хойд %d, өмнөд %d тариаланч, горим: %s\n", n, s,
	       fair ? "fair (8.31, starvation-гүй)" : "basic (8.30)");

	pthread_t th[200];
	struct farmer f[200];
	int k = 0;
	for (int i = 0; i < n; i++, k++)
		f[k] = (struct farmer){ NORTH, i + 1, (unsigned)time(NULL) + k };
	for (int i = 0; i < s; i++, k++)
		f[k] = (struct farmer){ SOUTH, i + 1, (unsigned)time(NULL) + k };
	for (int i = 0; i < k; i++)
		pthread_create(&th[i], NULL, farmer, &f[i]);
	for (int i = 0; i < k; i++)
		pthread_join(th[i], NULL);

	printf("Бүгд гарлаа (%.2f с). Эсрэг чиглэл зэрэг гүүрэн дээр: %d удаа → %s\n",
	       now(), violations, violations ? "АЛДАА" : "deadlock-гүй, зөв");
	return violations != 0;
}
