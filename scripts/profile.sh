#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"/..
RESULTS_DIR=${RESULTS_DIR:-results}

OUT="$RESULTS_DIR/resource_usage.log"
> "$OUT"
N=2

for i in $(seq 1 "$N"); do
  echo "=== Run #$i ===" >>"$OUT"
  /usr/bin/time -f "\
User_time(s): %U
Sys_time(s): %S
Max_RSS(KB): %M
Voluntary_CS: %w
Involuntary_CS: %c
" ./apartman >/dev/null 2>>"$OUT"
  echo >>"$OUT"
done

echo "✔ Profil testi tamamlandı: $OUT"

