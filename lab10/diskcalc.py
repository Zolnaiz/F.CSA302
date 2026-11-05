#!/usr/bin/env python3
"""
Лаб 10 — Practice Exercise 11.7-ийн тооцоо ба номын жишээгээр (§11.2) FCFS/SCAN/C-SCAN-ийн
үйлчлэх дарааллыг алхам алхмаар гаргана (disk_sched.c-ээс тусдаа шалгалт).

    python3 diskcalc.py
    python3 diskcalc.py 2150 98 183 37 ...   # өөр толгой/хүсэлтээр (5000 цилиндр)
"""
import sys


def order(policy, head, req, cyl):
    """Толгойн очих цэгүүдийн дараалал (зах руу очих, буцахыг оруулаад)."""
    if policy == "FCFS":
        return list(req)
    low = sorted(r for r in req if r < head)
    high = sorted(r for r in req if r >= head)
    if policy == "SCAN":                     # эхлээд 0 руу
        return low[::-1] + [0] + high
    # C-SCAN: дээш зах хүртэл, 0 руу буцаад дахин дээш
    return high + [cyl - 1] + ([0] + low if low else [])


def movement(head, path):
    total = 0
    for p in path:
        total += abs(p - head)
        head = p
    return total


if len(sys.argv) > 2:
    head, req, cyl = int(sys.argv[1]), [int(x) for x in sys.argv[2:]], 5000
else:
    head, req, cyl = 53, [98, 183, 37, 122, 14, 124, 65, 67], 200
print(f"Толгой {head}, хүсэлт {req}, цилиндр 0..{cyl - 1}")
for pol in ("FCFS", "SCAN", "C-SCAN"):
    path = order(pol, head, req, cyl)
    print(f"  {pol:<6} {movement(head, path):>6}  {head} → " + " → ".join(map(str, path)))

if len(sys.argv) > 2:
    sys.exit()

# 11.7: хэмжээ — хоёртын (KB = 2^10, MB = 2^20 байт), хурд — аравтын (MB/с = 10^6 байт/с).
print("11.7:")
devices = {                                  # хүлээлт (с), урсгалын хурд (байт/с)
    "кэш (L2)": (8e-9, 800e6),
    "санах ой": (60e-9, 80e6),
    "диск": (15e-3, 5e6),
    "соронзон хальс": (60.0, 2e6),
}
lat, rate = devices["диск"]
for name, size in (("512 B", 512), ("8 KB", 8 << 10), ("1 MB", 1 << 20), ("16 MB", 16 << 20)):
    eff = size / (lat + size / rate)
    print(f"  a/b диск {name:>6}: үр ашигтай хурд {eff / 1e3:>9.1f} KB/с, ашиглалт {eff / rate:7.2%}")
# ашиглалт = x/(L·R + x) ≥ 0.25  ⇔  x ≥ L·R/3
for name, (lat, rate) in devices.items():
    x = lat * rate / 3
    print(f"  c/e {name:<15} 25%-д хүрэх хамгийн бага хэмжээ ≈ {x:,.2f} байт")
