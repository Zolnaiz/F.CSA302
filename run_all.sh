#!/usr/bin/env bash
# Бүх лабыг хөрвүүлж, тестлэнэ — Ubuntu / WSL / GitHub Actions дээр ижилхэн.
#   bash run_all.sh            # FAIL байвал exit 1
# Шаардлага: gcc, make, python3, JDK (javac), OpenMP (gcc-д дагалддаг).
set -u
cd "$(dirname "$0")"
B=$(mktemp -d)
trap 'rm -rf "$B"' EXIT
fail=0

# expect "<нэр>" "<гаралтад байх ёстой текст>" команд...
expect() {
  local name=$1 want=$2 out code
  shift 2
  out=$("$@" 2>&1); code=$?
  if [ $code -eq 0 ] && [[ $out == *"$want"* ]]; then
    printf 'PASS  %s\n' "$name"
  else
    printf 'FAIL  %s (exit %d)\n' "$name" "$code"
    printf '%s\n' "$out" | tail -8 | sed 's/^/      /'
    fail=$((fail + 1))
  fi
}

echo "== Лаб 1 — Hello world (C, Java)"
expect "C hello"    "Hello" bash -c "gcc -Wall -o '$B/hello' lab1/hello.c && '$B/hello'"
expect "Java hello" "Hello" bash -c "javac -d '$B' lab1/HelloWorld.java && java -cp '$B' HelloWorld"

echo "== Лаб 2 — FileCopy (POSIX)"
expect "test.sh: бүх тохиолдол PASS" "PASS" bash -c 'out=$(bash lab2/test.sh); echo "$out"; ! grep -q "^FAIL" <<<"$out"'

echo "== Лаб 3 — процесс, IPC"
expect "make" "" make -s -C lab3
expect "collatz 35 → 35 ... 1" "35" bash -c './lab3/collatz 35'
expect "collatz буруу оролт → алдаа" "Usage" bash -c '! ./lab3/collatz'

echo "== Лаб 4 — урсгал"
expect "make (OpenMP орно)" "" make -s -C lab4
expect "javac: Date/Echo сервер" "" javac -d "$B" lab4/DateClient.java lab4/DateServerMT.java lab4/EchoServerMT.java

echo "== Лаб 5 — CPU хуваарилалт"
expect "sched_sim.py" "avg" python3 lab5/sched_sim.py

echo "== Лаб 6 — синхрончлол"
expect "race: уралдаан илэрнэ, mutex/monitor: 6/6 OK" "" bash -c '
  out=$(make -s -C lab6 test); echo "$out"
  grep -q "^race.*RACE CONDITION DETECTED" <<<"$out" &&
  [ "$(grep -E "^(mutex|monitor)" <<<"$out" | grep -c "OK - no race")" -eq 6 ]'

echo
if [ $fail -eq 0 ]; then echo "Бүгд PASS"; else echo "$fail FAIL"; fi
exit $((fail > 0))
