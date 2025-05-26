#!/usr/bin/env bash
set -euo pipefail
# 1) Proje köküne geç
cd "$(dirname "$0")"/..
# 2) results dizini (env’den veya default)
RESULTS_DIR=${RESULTS_DIR:-results}
# 3) summary.csv’i baştan temizle
> "$RESULTS_DIR/summary.csv"
echo "Run,TotalTime,Fastest,Slowest" > "$RESULTS_DIR/summary.csv"
# 4) Tek seferlik derleme
make clean && make all > /dev/null
# Döngü sayısı
N=2
# 5) Döngü
for i in $(seq 1 $N); do
  ./apartman > /dev/null
  TOTAL=$(grep "Simülasyon toplam süresi" report.txt | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
  FAST=$(grep "En hızlı daire" report.txt     | sed -E 's/.*\(([0-9]+\.[0-9]+) s\).*/\1/')
  SLOW=$(grep "En yavaş daire" report.txt     | sed -E 's/.*\(([0-9]+\.[0-9]+) s\).*/\1/')
  echo "$i,$TOTAL,$FAST,$SLOW" >> "$RESULTS_DIR/summary.csv"
done
echo "✔ Summary testi tamamlandı: $RESULTS_DIR/summary.csv"

