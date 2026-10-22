# Лаб №2 — FileCopy.exe-ийн Win32 API дуудалтуудыг gdb-ээр мөрдөх (strace-ийн оронд).
# Ажиллуулах (PowerShell, lab2 хавтсанд):
#   gcc -g -static -o FileCopy.exe FileCopy_win.c
#   gdb -q -batch -x trace_win.gdb FileCopy.exe
# Оролт gdb_input.txt-ээс уншигдана (эх файл, хүлээн авах файл).
# Breakpoint-ийг дуудалтын ДАРААХ мөрөнд тавьсан тул буцаасан утга харагдана.

set pagination off
set print thread-events off

break 75
commands
  silent
  printf "GetFileAttributesA(\"%s\")                          = 0x%lx\n", src, attr
  continue
end

break 87
commands
  silent
  printf "CreateFileA(\"%s\", GENERIC_READ, OPEN_EXISTING)  = handle %p\n", src, in
  continue
end

break 123
commands
  silent
  printf "CreateFileA(\"%s\", GENERIC_WRITE, CREATE_ALWAYS) = handle %p\n", dst, out
  continue
end

break 141
commands
  silent
  printf "ReadFile(%p, buf, 4096)                            = %lu bytes\n", in, nread
  continue
end

break 148
commands
  silent
  printf "WriteFile(%p, buf, %lu)                             = %lu bytes\n", out, nread, nwritten
  continue
end

break 152
commands
  silent
  printf "CloseHandle(%p)\n", in
  continue
end

break 156
commands
  silent
  printf "CloseHandle(%p)\n", out
  continue
end

run < gdb_input.txt
