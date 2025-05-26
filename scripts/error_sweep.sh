#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"/..
RESULTS_DIR=${RESULTS_DIR:-results}
N=5

> "$RESULTS_DIR/error_sweep.csv"
echo "ErrorRate,Run,TotalTime" > "$RESULTS_DIR/error_sweep.csv"

for ERR in 0 5 10 20; do
  make clean
  make all CFLAGS="-Wall -pthread -DERROR_RATE=${ERR}" > /dev/null
  for RUN in $(seq 1 $N); do
    ./apartman > /dev/null
    T=$(grep "Simülasyon toplam süresi" report.txt \
         | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
    echo "${ERR},${RUN},${T}" >> "$RESULTS_DIR/error_sweep.csv"
  done
done

echo "✔ Hata oranı sweep testi tamamlandı: $OUT"

