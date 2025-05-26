// Proje ödevinde her yazılan kod için yorum yazmanız gerekmektedir. – Hocamızın notu

#ifndef BINA_H
#define BINA_H

// 🍀 Standart kütüphaneler
#include <stdio.h>      // Giriş/çıkış işlemleri için
#include <stdlib.h>     // Genel yardımcı fonksiyonlar (malloc, free, exit)
#include <unistd.h>     // fork, usleep, vb. POSIX sistem çağrıları
#include <pthread.h>    // POSIX thread API’si
#include <sys/wait.h>   // wait ve ilgili makrolar
#include <semaphore.h>  // POSIX semaphore API’si
#include <time.h>       // time, srand, struct tm
#include <string.h>     // memcpy, memset, strcmp, vb.
// ⏱️ Performans ölçümü için timeval
#include <sys/time.h>   // gettimeofday, struct timeval
#include <stdarg.h>     // va_list ve variadic fonksiyonlar

// 📐 Sabitler
#define KAT_SAYISI        10   // Toplam kat sayısı
#define DAIRE_SAYISI      4    // Her katta kaç daire
#define TESISATCI_SAYISI  2    // Su tesisatçısı sayısı
#define ELEKTRIKCI_SAYISI 2    // Elektrikçi sayısı

// 🎨 ANSI renk makroları (konsol çıktısını renklendirmek için)
#define RESET   "\033[0m"
#define GRAY    "\033[1;30m"
#define BLUE    "\033[1;34m"
#define YELLOW  "\033[1;33m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define PURPLE  "\033[1;35m"  // Su baskını durumunda gösterilecek renk

// 🔐 Log dosyası işaretçisi (renksiz log yazmak için extern tanımı)
extern FILE *log_fp;

// ➜ Renkli konsol + renksiz log yazan printf makrosu
void log_console(const char *fmt, ...);
#define printf(...) log_console(__VA_ARGS__)

// 🔧 Senkronizasyon objeleri (extern tanımlar; bina.c içinde init edilir)
extern sem_t elektrikci_sem;                       // Elektrikçi semaforu
extern int elektrikciler[ELEKTRIKCI_SAYISI];       // Elektrikçi durumu dizisi
extern pthread_mutex_t elektrikci_kilit;           // Elektrikçi durumu için mutex

extern sem_t su_tesisatcisi_sem;                   // Su tesisatçısı semaforu
extern int tesisatcilar[TESISATCI_SAYISI];         // Tesisatçı durumu dizisi
extern pthread_mutex_t tesisatci_kilit;            // Tesisatçı durumu için mutex

extern sem_t itfaiyeci_sem;                        // İtfaiyeci semaforu (binary)
extern sem_t su_cekici_sem;                        // Su çekici semaforu (binary)

extern pthread_mutex_t daire_kilitleri[KAT_SAYISI][DAIRE_SAYISI]; // Daire kilitleri matrisi
extern int daire_durumlari[KAT_SAYISI][DAIRE_SAYISI];           // Daire durum matris
extern pthread_mutex_t yazici;                     // Konsol + log yazma mutex

// ⏱️ Zaman ölçümü için diziler (mmap ile allocate edilir)
extern struct timeval (*start_times)[DAIRE_SAYISI];
extern struct timeval (*end_times)  [DAIRE_SAYISI];

// 👷 Aktif işçi sayacı ve kilidi (thread-safe sayaç için)
extern int active_workers;
extern pthread_mutex_t worker_kilit;

// 🛠️ Fonksiyon prototipleri
void binayi_ciz(void);                             // ASCII bina çizim fonksiyonu
int tesisatci_al(void);                            // Boşta tesisatçı al
void tesisatci_bosalt(int i);                      // Tesisatçı serbest bırak
int elektrikci_al(void);                           // Boşta elektrikçi al
void elektrikci_bosalt(int i);                     // Elektrikçi serbest bırak
void daire_durum_guncelle(int kat, int daire, int tur); // UI ve durum güncelle
void karistir(int* dizi, int boyut);               // Fisher–Yates karıştırma
void* daire_insa(void* arg);                       // Thread fonksiyonu
void kat_insa(int kat_no);                         // Process içi daire thread yönetimi

#endif // BINA_H
