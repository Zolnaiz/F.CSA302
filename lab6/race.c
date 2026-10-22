/*
 * 6.33 — Номын анхны код: уралдааны нөхцөлтэй (race condition).
 *
 * a. Уралдаанд оролцож буй өгөгдөл: available_resources (дундын глобал хувьсагч).
 * b. Уралдаан гарах байрлал:
 *      decrease_count(): (1) "if (available_resources < count)" шалгалт ба
 *                        (2) "available_resources -= count" хасалтын хооронд —
 *                        хоёр урсгал зэрэг шалгалтыг давж, нөөцийг хэтрүүлэн авна.
 *                        Мөн хасалт өөрөө (унших-өөрчлөх-бичих) атомар биш.
 *      increase_count(): "available_resources += count" — атомар биш, нэмэлт алдагдана.
 *
 *   make race && ./race
 */
#include "resource_manager.h"

int available_resources = MAX_RESOURCES;
const char *impl_name = "race   — номын анхны код (синхрончлолгүй)";

/* decrease available resources by count resources */
/* return 0 if sufficient resources available, otherwise return -1 */
int decrease_count(int count)
{
    if (available_resources < count)        /* (1) шалгах */
        return -1;
    else {
        CONTEXT_SWITCH_WINDOW();            /* ← энд өөр урсгал мөн шалгалтыг давж болно */
        available_resources -= count;       /* (2) хасах */
        return 0;
    }
}

/* increase available resources by count */
int increase_count(int count)
{
    /* "available_resources += count" нь машины түвшинд 3 алхам (§6.1 counter++):
       регистрт унших → нэмэх → буцааж бичих. Дунд нь сэлгэлт гарвал нэмэлт алдагдана. */
    int reg = available_resources;
    CONTEXT_SWITCH_WINDOW();
    available_resources = reg + count;
    return 0;
}

int get_available(void) { return available_resources; }
