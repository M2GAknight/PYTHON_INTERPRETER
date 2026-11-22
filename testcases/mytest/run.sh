#!/usr/bin/env bash
# 跑 mytest 下所有自造用例
# 用法: bash testcases/mytest/run.sh        只跑
#       bash testcases/mytest/run.sh -b     先编译再跑
set -u
cd "$(dirname "$0")" || exit 1
BIN=../../build/code

if [ "${1:-}" = "-b" ]; then
  ( cd ../.. && cmake -S . -B build >/dev/null && cmake --build build -j ) || exit 1
fi
[ -x "$BIN" ] || { echo "没有 $BIN，先 bash testcases/mytest/run.sh -b"; exit 1; }

pass=0
fail=0
for f in case*.in; do
  n=${f%.in}
  if "$BIN" < "$f" | diff -q - "$n.out" >/dev/null 2>&1; then
    pass=$((pass + 1))
    printf 'ok   %s\n' "$n"
  else
    fail=$((fail + 1))
    printf 'FAIL %s\n' "$n"
    "$BIN" < "$f" | diff -u "$n.out" - | head -15 | sed 's/^/     /'
  fi
done
echo
echo "pass=$pass fail=$fail"
