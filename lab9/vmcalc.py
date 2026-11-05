#!/usr/bin/env python3
"""
Лаб 9 — Practice Exercises 10.4, 10.5, 10.7, 10.8, 10.9-ийн тооцоог шалгана
(page_replace.c-ээс тусдаа, хоёр дахь хэрэгжүүлэлт).

    python3 vmcalc.py
"""


def faults(ref, frames, policy):
    mem, n = [], 0
    for i, p in enumerate(ref):
        if p in mem:
            if policy == "LRU":              # сүүлд хандсаныг жагсаалтын төгсгөлд
                mem.remove(p)
                mem.append(p)
            continue
        n += 1
        if len(mem) == frames:
            if policy == "OPT":
                rest = ref[i + 1:]
                victim = max(mem, key=lambda q: rest.index(q) if q in rest else len(rest))
                mem.remove(victim)
            else:                            # FIFO, LRU: жагсаалтын эхнийх
                mem.pop(0)
        mem.append(p)
    return n


POLICIES = ("LRU", "FIFO", "OPT")

book = [7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1]
print("§10.4 номын жишээ (3 фрэйм):", {p: faults(book, 3, p) for p in POLICIES})

r8 = [1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6]
print("10.8:  фрэйм  " + "  ".join(f"{f:>2}" for f in range(1, 8)))
for p in POLICIES:
    print(f"      {p:<5}  " + "  ".join(f"{faults(r8, f, p):>2}" for f in range(1, 8)))

r9 = [7, 2, 3, 1, 2, 5, 3, 4, 6, 7, 7, 1, 0, 5, 4, 6, 2, 3, 0, 1]
print("10.9 (3 фрэйм):", {p: faults(r9, 3, p) for p in POLICIES})

# 10.5: 12 бит хаяг, 256 байт хуудас → дээд 4 бит хуудас, доод 8 бит офсет
table = {0: None, 1: 0x2, 2: 0xC, 3: 0xA, 4: None, 5: 0x4, 6: 0x3, 7: None, 8: 0xB, 9: 0x0}
free = [0xD, 0xE, 0xF]
print("10.5:")
for va in (0x9EF, 0x111, 0x700, 0x0FF):
    page, off = va >> 8, va & 0xFF
    note = ""
    if table[page] is None:                  # page fault → чөлөөт жагсаалтын толгой
        table[page] = free.pop(0)
        note = "  (page fault)"
    print(f"  {va:03X} → хуудас {page}, офсет {off:02X} → {table[page] << 8 | off:03X}{note}")

# 10.7: int A[100][100], A[0][0] хаяг 200, хуудас 200 үг → мөр бүр 100 үг, хуудсанд 2 мөр.
# Код 0-р хуудсанд (1 фрэйм эзэлнэ), өгөгдөлд 2 фрэйм, LRU.
def array_faults(order):
    pages = [(200 + i * 100 + j) // 200 for i, j in order]
    return faults(pages, 2, "LRU")


print("10.7 a (j гадна):", array_faults((i, j) for j in range(100) for i in range(100)))
print("10.7 b (i гадна):", array_faults((i, j) for i in range(100) for j in range(100)))

# 10.4: мкс-ээр. Дугуй 3000 эрг/мин → 20 мс/эрг, дундаж эргэлтийн хүлээлт 10 мс;
# 1000 үг / 10^6 үг/с = 1 мс → хуудас дамжуулах 11 мс = 11000 мкс.
page_io = 10_000 + 1_000
t = (0.99 * 1                                # өөрийн хуудас
     + 0.01 * 0.8 * 2                        # өөр хуудас, санах ойд байгаа
     + 0.01 * 0.2 * (2 + page_io)            # page fault: шинэ хуудсыг унших
     + 0.01 * 0.2 * 0.5 * page_io)           # 50%-д солигдох хуудсыг бичих
print(f"10.4: үр ашигтай зааврын хугацаа ≈ {t:.3f} мкс")
