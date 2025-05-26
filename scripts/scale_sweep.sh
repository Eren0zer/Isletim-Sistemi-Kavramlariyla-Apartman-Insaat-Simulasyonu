#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"/..
RESULTS_DIR=${RESULTS_DIR:-results}
> "$RESULTS_DIR/scale_sweep.csv"
echo "FloorsxUnits,TotalTime" > "$RESULTS_DIR/scale_sweep.csv"


for K in 10 15 20; do
  for D in 4 6 8; do
    sed -E "s/#define KAT_SAYISI +[0-9]+/#define KAT_SAYISI ${K}/"     -i bina.h
    sed -E "s/#define DAIRE_SAYISI +[0-9]+/#define DAIRE_SAYISI ${D}/" -i bina.h
    make clean && make all > /dev/null
    T=$(./apartman > /dev/null \
         && grep "Simülasyon toplam süresi" report.txt \
                | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
    echo "${K}x${D},${T}" >> "$RESULTS_DIR/scale_sweep.csv"
  done
done

echo "✔ Ölçek testi tamamlandı: $RESULTS_DIR/scale_sweep.csv"

