#!/usr/bin/env bash
# Лаб бүрийн ЗААВАР.md → labN/report/labN_tailbar.pdf, мөн лаб 1, 2-ын тайлан.
#   bash tools/build_guides.sh          # бүгд
#   bash tools/build_guides.sh 9 10     # сонгосон лабууд
# Шаардлага: texlive-xetex texlive-latex-extra fonts-dejavu python3
set -euo pipefail
cd "$(dirname "$0")/.."
labs=("$@"); [ ${#labs[@]} -eq 0 ] && labs=(1 2 3 4 5 6 7 8 9 10)

python3 tools/guides.py "${labs[@]}"
work=$(mktemp -d); trap 'rm -rf "$work"' EXIT
for n in "${labs[@]}"; do
  docs=("lab${n}_tailbar"); [ -f "lab$n/report/lab${n}_report.tex" ] && [ "$n" -le 2 ] && docs+=("lab${n}_report")
  for doc in "${docs[@]}"; do
    cp "lab$n/report/$doc.tex" "$work/"
    for _ in 1 2; do                     # 2 удаа — агуулгын хуудасны дугаар
      (cd "$work" && xelatex -interaction=nonstopmode -halt-on-error "$doc.tex" > /dev/null) \
        || { tail -30 "$work/$doc.log"; exit 1; }
    done
    if grep -q "Missing character" "$work/$doc.log"; then
      grep "Missing character" "$work/$doc.log" | sort -u; exit 1
    fi
    cp "$work/$doc.pdf" "lab$n/report/"
    echo "lab$n/report/$doc.pdf: $(pdfinfo "$work/$doc.pdf" | awk '/^Pages/ {print $2}') хуудас"
  done
  rm -f "lab$n/report/lab${n}_tailbar.tex"
done
