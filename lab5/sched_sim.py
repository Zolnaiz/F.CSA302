"""Лаб №5 — Бүлэг 5-ын дасгал 5.3, 5.4, 5.5-ын хариуг симуляцаар шалгах.

Ажиллуулах:  python sched_sim.py
FCFS / SJF / nonpreemptive priority / RR болон preemptive priority + RR (5.5)
алгоритмуудын Gantt дараалал, эргэлтийн (TAT) ба хүлээлтийн (WT) хугацааг хэвлэнэ.
Өөр өгөгдлөөр турших бол доорх p53 / p54 / p55 жагсаалтыг засна:
(нэр, ирэх хугацаа, burst, эрэмбэ).
"""
from fractions import Fraction as F


def nonpreemptive(procs, key, idle_until=0):
    """procs: list of (name, arrival, burst, prio). key: selection key among ready."""
    t, done, gantt, fin = F(idle_until), set(), [], {}
    if idle_until:
        gantt.append(("idle", F(0), F(idle_until)))
    while len(done) < len(procs):
        ready = [p for p in procs if p[0] not in done and p[1] <= t]
        if not ready:
            nxt = min(p[1] for p in procs if p[0] not in done)
            gantt.append(("idle", t, nxt)); t = nxt; continue
        p = min(ready, key=key)
        gantt.append((p[0], t, t + p[2])); t += p[2]; fin[p[0]] = t; done.add(p[0])
    return gantt, fin


def rr(procs, q):
    from collections import deque
    rem = {p[0]: p[2] for p in procs}; qd = deque(p[0] for p in procs)
    t, gantt, fin = 0, [], {}
    while qd:
        n = qd.popleft(); run = min(q, rem[n])
        gantt.append((n, t, t + run)); t += run; rem[n] -= run
        if rem[n]: qd.append(n)
        else: fin[n] = t
    return gantt, fin


def prio_rr(procs, q):
    """Preemptive priority, RR among equal priority; preempted -> end of queue."""
    arr = {p[0]: p[1] for p in procs}; prio = {p[0]: p[3] for p in procs}
    rem = {p[0]: p[2] for p in procs}; queue, t, cur, used = [], 0, None, 0
    gantt, fin = [], {}
    end = 200
    while t < end and len(fin) < len(procs):
        for p in procs:
            if p[1] == t: queue.append(p[0])
        if cur:
            best = max((prio[x] for x in queue), default=-1)
            if best > prio[cur]:
                queue.append(cur); cur = None
            elif used == q and any(prio[x] == prio[cur] for x in queue):
                queue.append(cur); cur = None
            elif used == q:
                used = 0
        if cur is None and queue:
            best = max(prio[x] for x in queue)
            cur = next(x for x in queue if prio[x] == best); queue.remove(cur); used = 0
        name = cur or "idle"
        if gantt and gantt[-1][0] == name: gantt[-1] = (name, gantt[-1][1], t + 1)
        else: gantt.append((name, t, t + 1))
        t += 1
        if cur:
            rem[cur] -= 1; used += 1
            if rem[cur] == 0: fin[cur] = t; cur = None
    return gantt, fin


def report(title, procs, gantt, fin):
    print(f"\n== {title}")
    print("  " + " | ".join(f"{n} {float(a):g}-{float(b):g}" for n, a, b in gantt))
    tat = {p[0]: fin[p[0]] - p[1] for p in procs}
    wt = {p[0]: tat[p[0]] - p[2] for p in procs}
    for p in procs:
        print(f"  {p[0]}: TAT={float(tat[p[0]]):g}  WT={float(wt[p[0]]):g}")
    print(f"  avg TAT={float(sum(tat.values()) / len(procs)):.3f}  avg WT={float(sum(wt.values()) / len(procs)):.3f}")


p53 = [("P1", F(0), 8, 0), ("P2", F(2, 5), 4, 0), ("P3", F(1), 1, 0)]
report("5.3a FCFS", p53, *nonpreemptive(p53, key=lambda p: p[1]))
report("5.3b SJF", p53, *nonpreemptive(p53, key=lambda p: (p[2], p[1])))
report("5.3c idle 1 + SJF", p53, *nonpreemptive(p53, key=lambda p: (p[2], p[1]), idle_until=1))

p54 = [("P1", 0, 2, 2), ("P2", 0, 1, 1), ("P3", 0, 8, 4), ("P4", 0, 4, 2), ("P5", 0, 5, 3)]
order = {p[0]: i for i, p in enumerate(p54)}
report("5.4 FCFS", p54, *nonpreemptive(p54, key=lambda p: order[p[0]]))
report("5.4 SJF", p54, *nonpreemptive(p54, key=lambda p: (p[2], order[p[0]])))
report("5.4 Priority", p54, *nonpreemptive(p54, key=lambda p: (-p[3], order[p[0]])))
report("5.4 RR q=2", p54, *rr(p54, 2))

p55 = [("P1", 0, 20, 40), ("P2", 25, 25, 30), ("P3", 30, 25, 30),
       ("P4", 60, 15, 35), ("P5", 100, 10, 5), ("P6", 105, 10, 10)]
g, f = prio_rr(p55, 10)
report("5.5 preemptive priority + RR q=10", p55, g, f)
busy = sum(b - a for n, a, b in g if n != "idle"); total = g[-1][2]
print(f"  CPU utilization = {busy}/{total} = {busy / total:.1%}")
