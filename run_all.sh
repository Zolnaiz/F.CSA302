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

echo "== Лаб 7 — түгжрэл"
expect "make (bridge, Transaction)" "" make -s -C lab7
expect "8.32 гүүр basic: зөрчил 0" "0 удаа" ./lab7/bridge 3 3
expect "8.31 гүүр fair: зөрчил 0" "0 удаа" ./lab7/bridge 4 4 fair
expect "8.33 ordered: нийт мөнгө хадгалагдсан" "хадгалагдсан: true" java -cp lab7 Transaction ordered
expect "8.33 naive: deadlock илэрнэ" "DEADLOCK" bash -c '! java -cp lab7 Transaction naive'
expect "банкирын алгоритм 8.9a аюулгүй биш" "АЮУЛГҮЙ БИШ" python3 lab7/banker.py

echo "== Лаб 8 — үндсэн санах ой"
expect "make" "" make -s -C lab8
expect "9.28 номын жишээ 19986 → 4 / 3602" "page number = 4" bash -c './lab8/addresses 19986 | grep -A1 "page number = 4" | grep -q "offset = 3602" && echo "page number = 4"'
expect "9.28 хамгийн том хаяг" "page number = 1048575" ./lab8/addresses 4294967295
expect "9.28 хэт том → алдаа" "does not fit" bash -c '! ./lab8/addresses 4294967296'
expect "9.6/9.7 тооцоо" "хуудас 1953, офсет 129" python3 lab8/memcalc.py

echo
if [ $fail -eq 0 ]; then echo "Бүгд PASS"; else echo "$fail FAIL"; fi
exit $((fail > 0))
