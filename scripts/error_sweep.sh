#!/usr/bin/env bash
set -euo pipefail
# 1) Script dosyasının bulunduğu klasörden proje kök dizinine geç
cd "$(dirname "$0")"/..
# 2) Sonuçların saklanacağı klasör (varsayılan: results/)
RESULTS_DIR=${RESULTS_DIR:-results}
# 3) Hata sweep testinin kaç kez çalıştırılacağını belirle
N=5
# 4) CSV dosyasını baştan temizle ve başlık satırını ekle
> "$RESULTS_DIR/error_sweep.csv"
echo "ErrorRate,Run,TotalTime" > "$RESULTS_DIR/error_sweep.csv"
# 5) Hata oranı parametrelerini deneme döngüsü
for ERR in 0 5 10 20; do
  # 5.1) Her bir hata oranı için temizle ve yeniden derle
  make clean
  make all CFLAGS="-Wall -pthread -DERROR_RATE=${ERR}" > /dev/null
  # 5.2) Belirlenen run sayısı kadar simülasyonu çalıştır
  for RUN in $(seq 1 $N); do
    # 5.2.1) Simülasyonu sessizce çalıştır
    ./apartman > /dev/null
    # 5.2.2) report.txt dosyasından toplam süreyi ayıkla
    T=$(grep "Simülasyon toplam süresi" report.txt \
         | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
    # 5.2.3) ErrorRate, Run numarası ve süreyi CSV dosyasına ekle
    echo "${ERR},${RUN},${T}" >> "$RESULTS_DIR/error_sweep.csv"
  done
done
# 6) Testin tamamlandığını bildir
echo "✔ Hata oranı sweep testi tamamlandı: $RESULTS_DIR/error_sweep.csv"
