#ifndef BINA_H
#define BINA_H

// 🍀 Standart kütüphaneler
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
// ⏱️ Performans ölçümü için timeval
#include <sys/time.h>
#include <stdarg.h>

// 📐 Sabitler
#define KAT_SAYISI 10
#define DAIRE_SAYISI 8
#define TESISATCI_SAYISI 3
#define ELEKTRIKCI_SAYISI 2

// 🎨 ANSI renk makroları
#define RESET   "\033[0m"
#define GRAY    "\033[1;30m"
#define BLUE    "\033[1;34m"
#define YELLOW  "\033[1;33m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define PURPLE  "\033[1;35m"  // su baskını için

// 🔐 Log dosyası işaretçisi
extern FILE *log_fp;

// ➜ Renkli konsol + renksiz log yazan printf makrosu
void log_console(const char *fmt, ...);
#define printf(...) log_console(__VA_ARGS__)

// 🔧 Senkronizasyon objeleri
extern sem_t elektrikci_sem;
extern int elektrikciler[ELEKTRIKCI_SAYISI];
extern pthread_mutex_t elektrikci_kilit;

extern sem_t su_tesisatcisi_sem;
extern int tesisatcilar[TESISATCI_SAYISI];
extern pthread_mutex_t tesisatci_kilit;

extern sem_t itfaiyeci_sem;
extern sem_t su_cekici_sem;

extern pthread_mutex_t daire_kilitleri[KAT_SAYISI][DAIRE_SAYISI];
extern int daire_durumlari[KAT_SAYISI][DAIRE_SAYISI];
extern pthread_mutex_t yazici;

// ⏱️ Zaman ölçümü için diziler
extern struct timeval (*start_times)[DAIRE_SAYISI];
extern struct timeval (*end_times)  [DAIRE_SAYISI];

// 👷 Aktif işçi sayacı
extern int active_workers;
extern pthread_mutex_t worker_kilit;

// 🛠️ Fonksiyon prototipleri
void binayi_ciz(void);
int tesisatci_al(void);
void tesisatci_bosalt(int i);
int elektrikci_al(void);
void elektrikci_bosalt(int i);
void daire_durum_guncelle(int kat, int daire, int tur);
void karistir(int* dizi, int boyut);
void* daire_insa(void* arg);
void kat_insa(int kat_no);

#endif

