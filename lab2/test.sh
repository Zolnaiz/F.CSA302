#!/bin/bash
# Лаб №2 — FileCopy (POSIX) тестүүд. Ажиллуулах: bash test.sh
cd "$(dirname "$0")"
gcc -Wall -Wextra -o FileCopy FileCopy.c || exit 1

T=$(mktemp -d)
echo "Hello from Lab 2" > "$T/input.txt"
head -c 5000000 /dev/urandom > "$T/big.bin"
: > "$T/empty.txt"

run() {  # run "<тайлбар>" "<stdin оролт>" <хүлээгдэх exit код>
    out=$(printf "$2" | ./FileCopy 2>&1); code=$?
    if [ "$code" -eq "$3" ]; then r=PASS; else r=FAIL; fi
    printf '%-4s | %-28s | %s\n' "$r" "$1" "$(echo "$out" | tail -1)"
}

run "Энгийн текст файл"      "$T/input.txt\n$T/out.txt\n"  0
cmp -s "$T/input.txt" "$T/out.txt" && echo "PASS | Агуулга ижил (cmp)"  || echo "FAIL | Агуулга зөрүүтэй"
run "5MB binary файл"        "$T/big.bin\n$T/big2.bin\n"   0
cmp -s "$T/big.bin" "$T/big2.bin" && echo "PASS | Binary ижил (cmp)"    || echo "FAIL | Binary зөрүүтэй"
run "Хоосон файл"            "$T/empty.txt\n$T/e2.txt\n"   0
run "Эх файл байхгүй"        "$T/nope.txt\n$T/x.txt\n"     1
run "Эх нь хавтас"           "$T\n$T/x.txt\n"              1
run "Эх = хүлээн авагч"      "$T/input.txt\n$T/input.txt\n" 1
run "Дарж бичих — n"         "$T/input.txt\n$T/out.txt\nn\n" 0
run "Дарж бичих — y"         "$T/input.txt\n$T/out.txt\ny\n" 0
run "Байхгүй хавтас руу"     "$T/input.txt\n$T/no/dir/x\n" 1
run "Хоосон нэр"             "\n"                           1

rm -rf "$T"
