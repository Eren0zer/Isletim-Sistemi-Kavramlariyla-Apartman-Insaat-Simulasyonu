#!/usr/bin/env bash
set -euo pipefail
# 1) Script’in konumundan proje kök dizinine geç
cd "$(dirname "$0")"/..
# 2) Sonuçların saklanacağı klasör (varsayılan: results/)
RESULTS_DIR=${RESULTS_DIR:-results}
# 3) Kaç kez çalıştırılacak (parametre sweep tekrarı)
N=2
# 4) Çıktı dosyasını baştan temizle ve başlık satırını ekle
> "$RESULTS_DIR/param_sweep.csv"
echo "Resource,Count,Run,TotalTime" > "$RESULTS_DIR/param_sweep.csv"
# 5) Her kaynak türü için (tesisatçı, elektrikçi) sayıyı 1–4 arası değiştir
for RES in TESISATCI_SAYISI ELEKTRIKCI_SAYISI; do
  # 5.1) Her bir kaynak sayısı değeri için döngü
  for COUNT in 1 2 3 4; do
    # bina.h içindeki makro tanımını güncelle
    sed -E "s/#define ${RES} +[0-9]+/#define ${RES} ${COUNT}/" -i bina.h
    # 5.2) Her COUNT değeri için N kez simülasyonu çalıştır
    for RUN in $(seq 1 $N); do
      # 5.2.1) Temizle ve yeniden derle
      make clean && make all > /dev/null
      # 5.2.2) Simülasyonu sessizce çalıştır
      ./apartman > /dev/null
      # 5.2.3) report.txt dosyasından toplam süreyi ayıkla
      T=$(grep "Simülasyon toplam süresi" report.txt \
           | sed -E 's/.*: ([0-9]+\.[0-9]+).*/\1/')
      # 5.2.4) Sonucu CSV dosyasına ekle
      echo "${RES},${COUNT},${RUN},${T}" >> "$RESULTS_DIR/param_sweep.csv"
    done
  done
done
# 6) Tamamlanma mesajı
echo "✔ Parametre sweep testi tamamlandı: $RESULTS_DIR/param_sweep.csv"
