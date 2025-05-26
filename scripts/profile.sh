#!/usr/bin/env bash
set -euo pipefail
# Proje ödevinde her yazılan kod için yorum yazmanız gerekmektedir. – Hocamızın notu
# 1) Script dosyasının bulunduğu klasörden proje kök dizinine geç
cd "$(dirname "$0")"/..
# 2) Sonuçların saklanacağı klasör (varsayılan: results/)
RESULTS_DIR=${RESULTS_DIR:-results}
# 3) Profil test sonuçlarının yazılacağı log dosyasını ayarla ve temizle
OUT="$RESULTS_DIR/resource_usage.log"
> "$OUT"
# 4) Simülasyonun kaç kez çalıştırılacağını belirle
N=5
# 5) Her bir testi çalıştırma döngüsü
for i in $(seq 1 "$N"); do
  # 5.1) Hangi run’ın yapıldığını log dosyasına ekle
  echo "=== Run #$i ===" >> "$OUT"
  # 5.2) 'apartman' ikilisini çalıştır, çıktılarını bastan gizleyip
  #      '/usr/bin/time' ile profil metriklerini log dosyasına yaz
  /usr/bin/time -f "\
User_time(s): %U
Sys_time(s): %S
Max_RSS(KB): %M
Voluntary_CS: %w
Involuntary_CS: %c
" ./apartman >/dev/null 2>> "$OUT"
  # 5.3) Log dosyasına bir satır boşluk ekleyerek ayrımı netleştir
  echo >> "$OUT"
done
# 6) Profil testinin tamamlandığını bildir
echo "✔ Profil testi tamamlandı: $OUT"
