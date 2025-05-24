# Apartman İnşaatı Simülasyonu (C dili - Process & Thread Modeli)

## İçindekiler
- [Proje Bilgisi](#proje-bilgisi)  
- [Amaç](#amac)  
- [Mimari](#mimari)  
- [Ornek Simulasyon Ciktisi](#ornek-simulasyon-ciktisi)  
- [Kullanim Talimatlari](#kullanim-talimatlari)  
- [Dosya Yapisi](#dosya-yapisi)  
- [Hata Senaryolari ve Yeniden Baslatmalar](#hata-senaryolari-ve-yeniden-baslatmalar)  
- [Gelistirme Notlari](#gelistirme-notlari)  
- [Proje Klasor Yapisi](#proje-klasor-yapisi)


---

## 📅 Proje Bilgisi

**Ders:** İşletim Sistemleri (2024-2025 Bahar Dönemi)
**Hazırlayanlar:**

* Yasin Ekici (21360859029)
* Eren Özer (22360859030)
<br>  **Grup No:** 8

---

## 📈 Amaç

Bu proje, C dili kullanarak çoklu işlem (multi-process) ve çoklu iş parçacık (multi-threading) yapıların senkronize edilmesini modellemektedir. Temel hedef, işletim sistemi dersinde öğrencilerin mutex, semaphore, process/thread senkronizasyonu gibi kavramları anlaması ve uygulamasıdır.

Simülasyon, bir apartmanın kat ve daire bazlı inşaatını temsil eder. Her kat ayrı bir **process**, her daire o kat içinde ayrı bir **thread** olarak tasarlanmıştır.

---

## 🏠 Sistem Mimarisi

* **Kat Sayısı:** 10

* **Daire Sayısı (Her Kat):** 4

* **Toplam Daire:** 40

* **Process Tabanlı Model:**

  * Her kat `fork()` ile oluşturulan ayrı bir process.
  * Katlar sırayla başlar: Üst kat, alt kat bitmeden başlamaz.

* **Thread Tabanlı Model:**

  * Her daire için bir `pthread_create()` ile thread oluşturulur.
  * Aynı kat içindeki daireler eşzamanlı başlar.

* **Kaynaklar ve Senkronizasyon:**

  * 2 su tesisatçısı (semaphore)
  * 2 elektrikçi (semaphore)
  * 1 su çekici (semaphore)
  * 1 itfaiyeci (semaphore)

* **Paylaşımlı Veriler:**

  * `daire_durumlari[][]` dizisi: her dairenin durumunu saklar.
  * `daire_kilitleri[][]`: her daire için mutex kilidi

* **Durum Kodları (Ekran Çıktıları):**

  * ⬜: Başlanmamış
  * 🟦: Su tesisatı tamamlanıyor
  * 🟨: Elektrik tesisatı tamamlanıyor
  * 🟩: Tamamlandı
  * 🟪: Su baskını
  * 🟥: Yangın

---

## ⚡ Örnek Simülasyon Çıktısı

```
=== KAT 1 İNŞAATI BAŞLADI ===
[Kat 1] Daire 1 -> İnşaat başladı (Thread 139902408881920)
[Kat 1] Daire 1 -> SU TESİSATÇISI [T1] GİRİYOR 💧
...
[Kat 1] Daire 1 -> SU TESİSATI TAMAMLANDI ✅ (T1)
...
=== KAT 1 TAMAMLANDI ===
```

Ayrıca tum olaylar `log.txt` dosyasına yazılır.

---

## 📆 Kullanım Talimatları

### Gereksinimler

* GCC
* POSIX pthread kütüphanesi
* Linux terminal ortamı

### Derleme

```bash
make
```

Oluşan çalıştırılabilir dosya `apartman` adını alır.

### Çalıştırma

```bash
./apartman
```

Simülasyon başladığında bina durumu terminale ASCII olarak yansıtılır.

---

## 🔍 Dosya Yapısı

* `main.c` : Process oluşturma ve kat senkronizasyonu
* `bina.c` : Ortak kaynaklar, güncelleme ve bina çizimi
* `daire.c` : Thread davranışları ve olay simülasyonu
* `bina.h` : Sabit tanımlar, fonksiyon prototipleri
* `Makefile` : Derleme komutları
* `log.txt` : Tüm log çıktıları (otomatik oluşur)

---

## ⚠️ Hata Senaryoları & Yeniden Başlatmalar

* **Yangın** (%10 ihtimal):

  * Elektrik tesisatı sırasında çıkar
  * İtfaiyeci müdahale eder, daire baştan başlar

* **Su Baskını** (%10 ihtimal):

  * Su tesisatında olur
  * Su çekici müdahale eder, tesisat yeniden başlar

---

## 🛠 Geliştirme Notları

* Tüm paylaşımlı kaynaklara mutex ve semaphore mekanizmaları eklendi.
* Yarış durumları, özel kilitlerle güvenli şekilde çözüldü.
* ASCII grafik arayüz ile kullanıcı dostu çıktı sağlandı.
* Geliştirme sürecinde, threading ve process oluşturmanın kombinasyonel zorluklarına karşı detaylı testler yapıldı.

---

## 📁 Proje Klasör Yapısı

```
apartman-simulasyonu/
├── bina.c
├── bina.h
├── daire.c
├── main.c
├── Makefile
└── log.txt (çalışma sırasında otomatik oluşturulur)
```


