#!/usr/bin/env python3
"""
Лаб 7 — банкирын алгоритм (OSC §8.6.3). Practice Exercise 8.3, 8.9-ийн хариуг шалгана.

    python3 banker.py
"""


def need(maxm, alloc):
    return [[m - a for m, a in zip(mr, ar)] for mr, ar in zip(maxm, alloc)]


def safety(avail, alloc, nd):
    """Аюулгүй дараалал (эсвэл None) ба алхам бүрийн Work-ийг буцаана."""
    work, finish, order, steps = list(avail), [False] * len(alloc), [], []
    progressed = True
    while progressed:
        progressed = False
        for i in range(len(alloc)):
            if not finish[i] and all(n <= w for n, w in zip(nd[i], work)):
                work = [w + a for w, a in zip(work, alloc[i])]
                finish[i] = True
                order.append(i)
                steps.append((i, list(work)))
                progressed = True
    return (order if all(finish) else None), steps, work, finish


def show(title, avail, alloc, maxm):
    nd = need(maxm, alloc)
    order, steps, work, finish = safety(avail, alloc, nd)
    print(f"--- {title}: Available = {tuple(avail)}")
    for i, row in enumerate(nd):
        print(f"  T{i}: Need = {tuple(row)}")
    for i, w in steps:
        print(f"  T{i} дуусна → Work = {tuple(w)}")
    if order is None:
        stuck = [f"T{i}" for i, f in enumerate(finish) if not f]
        print(f"  АЮУЛГҮЙ БИШ: {', '.join(stuck)} дуусах боломжгүй (Work = {tuple(work)})")
    else:
        print("  АЮУЛГҮЙ: <" + ", ".join(f"T{i}" for i in order) + ">")
    return order


# 8.3
alloc = [[0, 0, 1, 2], [1, 0, 0, 0], [1, 3, 5, 4], [0, 6, 3, 2], [0, 0, 1, 4]]
maxm = [[0, 0, 1, 2], [1, 7, 5, 0], [2, 3, 5, 6], [0, 6, 5, 2], [0, 6, 5, 6]]
show("8.3 a, b", [1, 5, 2, 0], alloc, maxm)
req = [0, 4, 2, 0]                     # 8.3 c: T1-ийн хүсэлт
nd = need(maxm, alloc)
print(f"--- 8.3 c: T1 хүсэлт {tuple(req)}; Request <= Need {tuple(nd[1])}:",
      all(r <= n for r, n in zip(req, nd[1])), "; Request <= Available (1, 5, 2, 0):",
      all(r <= a for r, a in zip(req, [1, 5, 2, 0])))
alloc_c = [row[:] for row in alloc]
alloc_c[1] = [a + r for a, r in zip(alloc[1], req)]
show("8.3 c (хүсэлтийг өгсөн гэж үзвэл)", [a - r for a, r in zip([1, 5, 2, 0], req)], alloc_c, maxm)

# 8.9
alloc = [[3, 0, 1, 4], [2, 2, 1, 0], [3, 1, 2, 1], [0, 5, 1, 0], [4, 2, 1, 2]]
maxm = [[5, 1, 1, 7], [3, 2, 1, 1], [3, 3, 2, 1], [4, 6, 1, 2], [6, 3, 2, 5]]
show("8.9 a", [0, 3, 0, 1], alloc, maxm)
show("8.9 b", [1, 0, 0, 2], alloc, maxm)
