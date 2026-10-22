/*
 * 6.34 — Нөөц менежерийг монитор ба condition variable-аар бичих.
 * Монитор = мутекс (нэг удаад нэг урсгал дотор нь) + condition variable (нөхцөл хүлээх).
 * decrease_count() нөөц хүрэлцэх хүртэл урсгалыг ТҮР ЗОГСООНО — дуудагч
 * "while (decrease_count(count) == -1);" гэж завгүй хүлээх (busy waiting) шаардлагагүй.
 *
 *   make monitor && ./monitor
 */
#include <pthread.h>
#include "resource_manager.h"

int available_resources = MAX_RESOURCES;
const char *impl_name = "monitor — 6.34, монитор + condition variable (хүлээнэ)";

static pthread_mutex_t monitor_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  enough_resources = PTHREAD_COND_INITIALIZER;

int decrease_count(int count)
{
    /* MAX-аас их хүсвэл хэзээ ч биелэхгүй тул үүрд хүлээхээс сэргийлнэ */
    if (count <= 0 || count > MAX_RESOURCES)
        return -1;

    pthread_mutex_lock(&monitor_lock);
    /* while — сэрсний дараа нөхцөлийг ДАХИН шалгана: өөр урсгал түрүүлж
       авсан байж болно, эсвэл spurious wakeup */
    while (available_resources < count)
        pthread_cond_wait(&enough_resources, &monitor_lock);   /* lock-оо чөлөөлж унтана */

    CONTEXT_SWITCH_WINDOW();
    available_resources -= count;
    pthread_mutex_unlock(&monitor_lock);
    return 0;
}

int increase_count(int count)
{
    if (count <= 0)
        return -1;

    pthread_mutex_lock(&monitor_lock);
    int reg = available_resources;
    CONTEXT_SWITCH_WINDOW();
    available_resources = reg + count;
    /* broadcast — хүлээж буй урсгалууд өөр өөр тоо хүсэж байгаа тул бүгдийг сэрээж
       тус бүр нөхцөлөө шалгана (signal-аар зөвхөн нэгийг сэрээвэл тэр нь хангалтгүй байж болно) */
    pthread_cond_broadcast(&enough_resources);
    pthread_mutex_unlock(&monitor_lock);
    return 0;
}

int get_available(void)
{
    pthread_mutex_lock(&monitor_lock);
    int v = available_resources;
    pthread_mutex_unlock(&monitor_lock);
    return v;
}
