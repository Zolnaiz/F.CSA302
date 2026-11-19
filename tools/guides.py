#!/usr/bin/env python3
"""
labN/ЗААВАР.md → labN/report/labN_tailbar.tex — лаб бүрт юу хийсэн, код хэрхэн ажилладаг,
хэрхэн ажиллуулах, багшийн асуултын хариултыг нэг PDF-д (tools/build_guides.sh хөрвүүлнэ).

    python3 tools/guides.py 1 2 3 ...
"""
import sys
from pathlib import Path

from md2tex import PREAMBLE, convert, inline

ROOT = Path(__file__).resolve().parent.parent

# DejaVu-д байхгүй emoji-г үгээр/тэмдгээр солино
EMOJI = {"✅": "✔", "⏰": "(хугацаа өнгөрсөн)", "🎥 ": "", "🎥": "", "️": ""}
# DejaVu Serif-д байхгүй, Sans-д байгаа тэмдэгтүүд
EXTRA = "".join(f"\\catcode`{c}=\\active \\def{c}{{{{\\symfont\\char\"{ord(c):X}}}}}\n" for c in "①②③④")


def build(n):
    src = ROOT / f"lab{n}" / "ЗААВАР.md"
    md = src.read_text(encoding="utf-8")
    for k, v in EMOJI.items():
        md = md.replace(k, v)
    title, body = convert(md, src.parent, src.parent / "report")
    head = (r"\begin{center}{\large F.CSM302 Үйлдлийн систем\par}\vspace{0.3em}"
            r"{\LARGE\bfseries " + inline(title) + r"\par}\vspace{0.4em}"
            r"{\large Хийсэн ажлын тайлбар ба хамгаалалтын заавар\par}\vspace{0.4em}"
            r"Гүйцэтгэсэн: Б.Билгүүн (B190910014)\par"
            r"\vspace{0.2em}{\small Код: \url{https://github.com/Zolnaiz/F.CSA302/tree/main/lab" + str(n) + r"}}\end{center}"
            "\n\\hrule\n\\tableofcontents\n\\medskip\\hrule\\medskip\n")
    pre = PREAMBLE.replace(r"\begin{document}", EXTRA + r"\begin{document}")
    dst = src.parent / "report" / f"lab{n}_tailbar.tex"
    dst.parent.mkdir(exist_ok=True)
    dst.write_text(pre + head + body + "\n\\end{document}\n", encoding="utf-8")
    print(f"{src.relative_to(ROOT)} → {dst.relative_to(ROOT)}")


if __name__ == "__main__":
    for arg in sys.argv[1:] or [str(i) for i in range(1, 11)]:
        build(int(arg))
