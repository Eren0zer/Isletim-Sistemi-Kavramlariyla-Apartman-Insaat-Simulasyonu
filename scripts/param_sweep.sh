#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"/..
RESULTS_DIR=${RESULTS_DIR:-results}
N=2
> "$RESULTS_DIR/param_sweep.csv"
echo "Resource,Count,Run,TotalTime" > "$RESULTS_DIR/param_sweep.csv"

for RES in TESISATCI_SAYISI ELEKTRIKCI_SAYISI; do
  for COUNT in 1 2 3 4; do
    sed -E "s/#define ${RES} +[0-9]+/#define ${RES} ${COUNT}/" -i bina.h
    for RUN in $(seq 1 $N); do
      make clean && make all > /dev/null
      ./apartman > /dev/null
      T=$(grep "Simülasyon toplam süresi" report.txt \
           | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
      echo "${RES},${COUNT},${RUN},${T}" >> "$RESULTS_DIR/param_sweep.csv"
    done
  done
done

echo "✔ Parametre sweep testi tamamlandı: $RESULTS_DIR/param_sweep.csv"

