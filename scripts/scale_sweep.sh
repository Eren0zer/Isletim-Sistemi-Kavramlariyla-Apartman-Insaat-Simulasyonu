#!/usr/bin/env bash
set -euo pipefail
# 1) Script dosyasının bulunduğu klasörden proje kök dizinine geç
cd "$(dirname "$0")"/..
# 2) Sonuçların saklanacağı klasör (varsayılan: results/)
RESULTS_DIR=${RESULTS_DIR:-results}
# 3) CSV dosyasını baştan temizle ve başlık satırını ekle
> "$RESULTS_DIR/scale_sweep.csv"
echo "FloorsxUnits,TotalTime" > "$RESULTS_DIR/scale_sweep.csv"
# 4) Kat ve daire sayısı kombinasyonlarını deneme döngüsü
for K in 10 15 20; do
  for D in 4 6 8; do
    # 4.1) bina.h içindeki makro tanımlarını K ve D ile güncelle
    sed -E "s/#define KAT_SAYISI +[0-9]+/#define KAT_SAYISI ${K}/"     -i bina.h
    sed -E "s/#define DAIRE_SAYISI +[0-9]+/#define DAIRE_SAYISI ${D}/" -i bina.h
    # 4.2) Temizle ve yeniden derle, simülasyonu sessizce çalıştır
    make clean && make all > /dev/null
    T=$(./apartman > /dev/null \
         && grep "Simülasyon toplam süresi" report.txt \
                | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
    # 4.3) Kombinasyon ve süreyi CSV dosyasına ekle
    echo "${K}x${D},${T}" >> "$RESULTS_DIR/scale_sweep.csv"
  done
done
# 5) Ölçek testinin tamamlandığını bildir
echo "✔ Ölçek testi tamamlandı: $RESULTS_DIR/scale_sweep.csv"
