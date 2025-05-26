#include "bina.h"

// 🔐 Log dosyası işaretçisi
FILE *log_fp;

// ➜ ANSI kaçış kodlarını atlayarak “plain” metin üreten yardımcı
static void strip_ansi(const char *src, char *dst) {
    while (*src) {
        if (*src == '\033' && src[1] == '[') {
            src += 2;
            while (*src && *src++ != 'm');
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// ➜ Konsola renkli, log dosyasına renksiz yazan printf
void log_console(const char *fmt, ...) {
    char buf[4096], plain[4096];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    // Konsola renkli
    fputs(buf, stdout);
    fflush(stdout);

    // Log için ANSI kodları at
    strip_ansi(buf, plain);
    fputs(plain, log_fp);
    fflush(log_fp);
}
// 🔌 Elektrikçi semaforu ve durumu
sem_t elektrikci_sem;
int elektrikciler[ELEKTRIKCI_SAYISI] = {0};
pthread_mutex_t elektrikci_kilit = PTHREAD_MUTEX_INITIALIZER;

// 🚰 Su tesisatçısı semaforu ve durumu
sem_t su_tesisatcisi_sem;
int tesisatcilar[TESISATCI_SAYISI] = {0};
pthread_mutex_t tesisatci_kilit = PTHREAD_MUTEX_INITIALIZER;

// 🚒 İtfaiyeci ve su çekici semaforları
sem_t itfaiyeci_sem;
sem_t su_cekici_sem;

// 🏗️ Daire kilitleri ve durum matrisi
pthread_mutex_t daire_kilitleri[KAT_SAYISI][DAIRE_SAYISI];
int daire_durumlari[KAT_SAYISI][DAIRE_SAYISI];
pthread_mutex_t yazici = PTHREAD_MUTEX_INITIALIZER;

// ⏱️ Performans ölçümü için zaman dizileri
struct timeval (*start_times)[DAIRE_SAYISI];
struct timeval (*end_times)  [DAIRE_SAYISI];

// 👷 Aktif işçi sayacı ve kilidi
int active_workers = 0;
pthread_mutex_t worker_kilit = PTHREAD_MUTEX_INITIALIZER;

/*
 * Boşta bir tesisatçıyı alır, yoksa -1 döner
 */
int tesisatci_al() {
    pthread_mutex_lock(&tesisatci_kilit);
    for (int i = 0; i < TESISATCI_SAYISI; i++) {
        if (tesisatcilar[i] == 0) {
            tesisatcilar[i] = 1;
            pthread_mutex_unlock(&tesisatci_kilit);
            return i;
        }
    }
    pthread_mutex_unlock(&tesisatci_kilit);
    return -1; // Ulaşılamaz durumda
}

/*
 * Tesisatçıyı serbest bırakır
 */
void tesisatci_bosalt(int i) {
    pthread_mutex_lock(&tesisatci_kilit);
    tesisatcilar[i] = 0;
    pthread_mutex_unlock(&tesisatci_kilit);
}

/*
 * Boşta bir elektrikçiyi alır, yoksa -1 döner
 */
int elektrikci_al() {
    pthread_mutex_lock(&elektrikci_kilit);
    for (int i = 0; i < ELEKTRIKCI_SAYISI; i++) {
        if (elektrikciler[i] == 0) {
            elektrikciler[i] = 1;
            pthread_mutex_unlock(&elektrikci_kilit);
            return i;
        }
    }
    pthread_mutex_unlock(&elektrikci_kilit);
    return -1;
}

/*
 * Elektrikçiyi serbest bırakır
 */
void elektrikci_bosalt(int i) {
    pthread_mutex_lock(&elektrikci_kilit);
    elektrikciler[i] = 0;
    pthread_mutex_unlock(&elektrikci_kilit);
}

/*
 * Daire durumunu günceller, UI'ı yeniden çizer
 * ve anlık aktif işçi sayısını ekrana basar
 */
void daire_durum_guncelle(int kat, int daire, int tur) {
    pthread_mutex_lock(&yazici);

    // Hata/yangın sonrası her iki iş bitince 'tamam' durumu
    if (tur == 1 && daire_durumlari[kat][daire] == 2)
        daire_durumlari[kat][daire] = 3;
    else if (tur == 2 && daire_durumlari[kat][daire] == 1)
        daire_durumlari[kat][daire] = 3;
    else
        daire_durumlari[kat][daire] = tur;

    // UI çizimini bozmadan çalıştırır
    binayi_ciz();

    // 👷 Aktif işçi sayısını göster
    printf("Aktif işçi sayısı: %d\n\n", active_workers);
    fflush(stdout);

    pthread_mutex_unlock(&yazici);
}

/*
 * ASCII tabanlı bina çizim fonksiyonu
 * (Buna kesinlikle dokunmayın!)
 */
void binayi_ciz() {
    system("clear");
    printf("\n🏢 BİNA DURUMU (⬜ Başlanmadı, 🟦 Su, 🟨 Elektrik, 🟩 Bitti, 🟪 Su Baskını, 🟥 Yangın)\n\n");

    // En üst aktif katı bul
    int aktif_kat = -1;
    for (int i = 0; i < KAT_SAYISI; i++)
        for (int j = 0; j < DAIRE_SAYISI; j++)
            if (daire_durumlari[i][j] != 0 && i > aktif_kat)
                aktif_kat = i;

    // Katları çiz
    for (int i = aktif_kat; i >= 0; i--) {
        printf("Kat %2d | ", i + 1);
        for (int j = 0; j < DAIRE_SAYISI; j++) {
            int d = daire_durumlari[i][j];
            if (i < aktif_kat) {
                // Alt katlar bitti gösterilir
                printf(GREEN "🟩 " RESET);
            } else {
                // Mevcut kat durumu
                switch (d) {
                    case -2: printf(PURPLE "🟪 " RESET); break;
                    case -1: printf(RED    "🟥 " RESET); break;
                    case  0: printf(GRAY   "⬜ " RESET); break;
                    case  1: printf(BLUE   "🟦 " RESET); break;
                    case  2: printf(YELLOW "🟨 " RESET); break;
                    case  3: printf(GREEN  "🟩 " RESET); break;
                }
            }
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}

/*
 * Fisher–Yates karıştırma algoritması
 */
void karistir(int* dizi, int boyut) {
    for (int i = boyut - 1; i > 0; i--) {
        int j   = rand() % (i + 1);
        int tmp = dizi[i];
        dizi[i] = dizi[j];
        dizi[j] = tmp;
    }
}

