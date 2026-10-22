/*
 * 6.33c — Уралдааны нөхцөлийг мутексээр засах.
 * Шалгалт ба хасалт/нэмэлтийг нэг эгзэгтэй мужид (critical section) оруулснаар
 * нэг удаад зөвхөн нэг урсгал available_resources-д хандана (mutual exclusion).
 * Нөөц хүрэлцэхгүй бол номын анхны интерфейсийн адил -1 буцаана.
 *
 *   make mutex && ./mutex
 */
#include <pthread.h>
#include "resource_manager.h"

int available_resources = MAX_RESOURCES;
const char *impl_name = "mutex  — 6.33c, мутекстэй (хүрэлцэхгүй бол -1)";

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int decrease_count(int count)
{
    if (count <= 0)
        return -1;

    pthread_mutex_lock(&lock);              /* ---- эгзэгтэй муж эхлэв ---- */
    if (available_resources < count) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    CONTEXT_SWITCH_WINDOW();                /* сэлгэлт гарсан ч бусад нь lock-д хүлээнэ */
    available_resources -= count;
    pthread_mutex_unlock(&lock);            /* ---- эгзэгтэй муж дуусав ---- */
    return 0;
}

int increase_count(int count)
{
    if (count <= 0)
        return -1;

    pthread_mutex_lock(&lock);
    int reg = available_resources;
    CONTEXT_SWITCH_WINDOW();
    available_resources = reg + count;
    pthread_mutex_unlock(&lock);
    return 0;
}

int get_available(void)
{
    pthread_mutex_lock(&lock);
    int v = available_resources;
    pthread_mutex_unlock(&lock);
    return v;
}
