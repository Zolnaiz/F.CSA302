#!/usr/bin/env python3
"""
Лаб 8 — Practice Exercise 9.6 (first/best/worst-fit) ба 9.7 (1 KB хуудас)-ыг шалгана.

    python3 memcalc.py
"""


def place(holes, procs, choose):
    """Динамик хуваарилалт (§9.2.2): сонгосон нүхнээс процессыг хувааж өгнө, үлдэгдэл нь нүх хэвээр."""
    holes = list(holes)
    out = []
    for p in procs:
        fits = [i for i, h in enumerate(holes) if h >= p]
        if not fits:
            out.append((p, None, None))
            continue
        i = choose(fits, holes)
        out.append((p, holes[i], holes[i] - p))
        holes[i] -= p
    return out, holes


holes = [300, 600, 350, 200, 750, 125]
procs = [115, 500, 358, 200, 375]
rules = {
    "First-fit": lambda fits, h: fits[0],
    "Best-fit": lambda fits, h: min(fits, key=lambda i: h[i]),
    "Worst-fit": lambda fits, h: max(fits, key=lambda i: h[i]),
}
for name, rule in rules.items():
    res, left = place(holes, procs, rule)
    parts = [f"{p} KB → " + (f"{h} KB нүх (үлдэгдэл {r})" if h else "БАГТАХГҮЙ (хүлээнэ)") for p, h, r in res]
    print(f"{name}:\n  " + "\n  ".join(parts) + f"\n  Эцсийн нүхнүүд: {left}")

print("9.7 (1 KB = 1024 байт хуудас):")
for a in (3085, 42095, 215201, 650000, 2000001):
    print(f"  {a:>8}: хуудас {a // 1024}, офсет {a % 1024}")
