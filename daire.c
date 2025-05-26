#include "bina.h"

/*
 * Her daire için çalışacak inşaat thread fonksiyonu
 */
void* daire_insa(void* arg) {
    int* bilgiler = (int*)arg;
    int kat_no    = bilgiler[0];
    int daire_no  = bilgiler[1];
    free(bilgiler);

    // ⏱️ Başlangıç zamanını kaydet
    gettimeofday(&start_times[kat_no-1][daire_no-1], NULL);

    // 👷 Aktif işçi sayısını artır
    pthread_mutex_lock(&worker_kilit);
    active_workers++;
    pthread_mutex_unlock(&worker_kilit);

    // Küçük bir başlangıç gecikmesi
    usleep(800000 + rand() % 500000);

    pthread_mutex_lock(&yazici);
    printf("  [Kat %d] Daire %d -> İnşaat başladı (Thread %lu)\n",
           kat_no, daire_no, pthread_self());
    fflush(stdout);
    pthread_mutex_unlock(&yazici);

    unsigned int seed    = time(NULL) ^ pthread_self();
    int su_bitti         = 0;
    int elektrik_bitti   = 0;

    // Su ve elektrik işlerini rastgele sırayla yap
    while (!su_bitti || !elektrik_bitti) {
        int ilk = rand_r(&seed) % 2;
        for (int i = 0; i < 2; i++) {
            int once = (ilk == 0 && i == 0) || (ilk == 1 && i == 1);

            // --- SU kısmı ---
            if (once && !su_bitti) {
                sem_wait(&su_tesisatcisi_sem);
                pthread_mutex_lock(&daire_kilitleri[kat_no-1][daire_no-1]);
                int t_no = tesisatci_al();

                pthread_mutex_lock(&yazici);
                printf("  [Kat %d] Daire %d -> SU TESİSATÇISI [T%d] GİRİYOR 💧\n",
                       kat_no, daire_no, t_no+1);
                fflush(stdout);
                pthread_mutex_unlock(&yazici);

                usleep(1500000 + rand_r(&seed) % 1000000);
                seed += rand_r(&seed);

                // %10 ihtimalle su baskını
                if (rand_r(&seed) % 100 < 10) {
                    sem_wait(&su_cekici_sem);
                    pthread_mutex_lock(&yazici);
                    printf(PURPLE
                           "  💦 [Kat %d] Daire %d → SU BASKINI! SU ÇEKİCİ GELİYOR... 🚿\n"
                           RESET, kat_no, daire_no);
                    daire_durumlari[kat_no-1][daire_no-1] = -2;
                    binayi_ciz();
                    fflush(stdout);
                    pthread_mutex_unlock(&yazici);

                    // Müdahale süresi
                    usleep(3000000);
                    sem_post(&su_cekici_sem);

                    // Baştan başlat
                    su_bitti = elektrik_bitti = 0;
                    pthread_mutex_lock(&yazici);
                    printf("  🔁 [Kat %d] Daire %d -> Tüm tesisat YENİDEN BAŞLIYOR...\n",
                           kat_no, daire_no);
                    pthread_mutex_unlock(&yazici);

                    pthread_mutex_unlock(&daire_kilitleri[kat_no-1][daire_no-1]);
                    sem_post(&su_tesisatcisi_sem);
                    tesisatci_bosalt(t_no);
                    break;
                }

                // Normal su işi tamamlandı
                pthread_mutex_lock(&yazici);
                printf("  [Kat %d] Daire %d -> SU TESİSATI TAMAMLANDI ✅ (T%d)\n",
                       kat_no, daire_no, t_no+1);
                pthread_mutex_unlock(&yazici);

                su_bitti = 1;
                daire_durum_guncelle(kat_no-1, daire_no-1, 1);
                pthread_mutex_unlock(&daire_kilitleri[kat_no-1][daire_no-1]);
                sem_post(&su_tesisatcisi_sem);
                tesisatci_bosalt(t_no);
            }

            // --- ELEKTRİK kısmı ---
            if (!once && !elektrik_bitti) {
                sem_wait(&elektrikci_sem);
                pthread_mutex_lock(&daire_kilitleri[kat_no-1][daire_no-1]);
                int e_no = elektrikci_al();

                pthread_mutex_lock(&yazici);
                printf("  [Kat %d] Daire %d -> ELEKTRİKÇİ [E%d] GİRİYOR ⚡\n",
                       kat_no, daire_no, e_no+1);
                fflush(stdout);
                pthread_mutex_unlock(&yazici);

                usleep(1500000 + rand_r(&seed) % 1000000);
                seed += rand_r(&seed);

                // %10 ihtimalle yangın
                if (rand_r(&seed) % 100 < 10) {
                    sem_wait(&itfaiyeci_sem);
                    pthread_mutex_lock(&yazici);
                    printf(RED
                           "  🔥 [Kat %d] Daire %d → YANGIN! İTFAİYECİ MÜDAHALE EDİYOR... 🧯\n"
                           RESET, kat_no, daire_no);
                    daire_durumlari[kat_no-1][daire_no-1] = -1;
                    binayi_ciz();
                    fflush(stdout);
                    pthread_mutex_unlock(&yazici);

                    // Müdahale süresi
                    usleep(3000000);
                    sem_post(&itfaiyeci_sem);

                    // Baştan başlat
                    su_bitti = elektrik_bitti = 0;
                    pthread_mutex_lock(&yazici);
                    printf("  🔁 [Kat %d] Daire %d -> Tüm tesisat YENİDEN BAŞLIYOR...\n",
                           kat_no, daire_no);
                    pthread_mutex_unlock(&yazici);

                    pthread_mutex_unlock(&daire_kilitleri[kat_no-1][daire_no-1]);
                    sem_post(&elektrikci_sem);
                    elektrikci_bosalt(e_no);
                    break;
                }

                // Normal elektrik işi tamamlandı
                pthread_mutex_lock(&yazici);
                printf("  [Kat %d] Daire %d -> ELEKTRİK TESİSATI TAMAMLANDI ✅ (E%d)\n",
                       kat_no, daire_no, e_no+1);
                pthread_mutex_unlock(&yazici);

                elektrik_bitti = 1;
                daire_durum_guncelle(kat_no-1, daire_no-1, 2);
                pthread_mutex_unlock(&daire_kilitleri[kat_no-1][daire_no-1]);
                sem_post(&elektrikci_sem);
                elektrikci_bosalt(e_no);
            }
        }
    }

    // ⏱️ Bitiş zamanını kaydet
    gettimeofday(&end_times[kat_no-1][daire_no-1], NULL);

    // 👷 Aktif işçi sayısını azalt
    pthread_mutex_lock(&worker_kilit);
    active_workers--;
    pthread_mutex_unlock(&worker_kilit);

    return NULL;
}

/*
 * Kat inşaatını yöneten fonksiyon
 */
void kat_insa(int kat_no) {
    pthread_t threads[DAIRE_SAYISI];
    int daireler[DAIRE_SAYISI];
    for (int j = 0; j < DAIRE_SAYISI; ++j)
      daireler[j] = j+1;

    karistir(daireler, DAIRE_SAYISI);

    pthread_mutex_lock(&yazici);
    printf("\n=== KAT %d İNŞAATI BAŞLADI (PID: %d) ===\n", kat_no, getpid());
    fflush(stdout);
    pthread_mutex_unlock(&yazici);

    for (int i = 0; i < DAIRE_SAYISI; i++) {
        int *bilgiler = malloc(2 * sizeof(int));
        bilgiler[0]   = kat_no;
        bilgiler[1]   = daireler[i];
        pthread_create(&threads[i], NULL, daire_insa, bilgiler);
    }

    for (int i = 0; i < DAIRE_SAYISI; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_lock(&yazici);
    printf("=== KAT %d TAMAMLANDI ===\n", kat_no);
    fflush(stdout);
    pthread_mutex_unlock(&yazici);
}

