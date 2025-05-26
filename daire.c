#include "bina.h"

/*
 * Her daire için çalışacak inşaat thread fonksiyonu
 */
void* daire_insa(void* arg) {
    // 1) Thread parametrelerini argümandan oku ve free et
    int* bilgiler = (int*)arg;
    int kat_no    = bilgiler[0];
    int daire_no  = bilgiler[1];
    free(bilgiler);

    // 2) Başlangıç zamanını paylaşımlı belleğe kaydet
    gettimeofday(&start_times[kat_no-1][daire_no-1], NULL);

    // 3) Aktif işçi sayısını artır (thread-safe)
    pthread_mutex_lock(&worker_kilit);
    active_workers++;
    pthread_mutex_unlock(&worker_kilit);

    // 4) Rastgele küçük bir başlangıç gecikmesi
    usleep(800000 + rand() % 500000);

    // 5) İnşaat başlangıç mesajını yazdır (konsol + log)
    pthread_mutex_lock(&yazici);
    printf("  [Kat %d] Daire %d -> İnşaat başladı (Thread %lu)\n",
           kat_no, daire_no, pthread_self());
    fflush(stdout);
    pthread_mutex_unlock(&yazici);

    // 6) Rastgele ve tekrar eden su/elektrik işleri için seed oluştur
    unsigned int seed    = time(NULL) ^ pthread_self();
    int su_bitti         = 0;
    int elektrik_bitti   = 0;

    // 7) Su ve elektrik işlerini rastgele sırayla yapana kadar döngü
    while (!su_bitti || !elektrik_bitti) {
        int ilk = rand_r(&seed) % 2;  // önce su mu önce elektrik mi
        for (int i = 0; i < 2; i++) {
            // hangi iş önce yapılacak?
            int once = (ilk == 0 && i == 0) || (ilk == 1 && i == 1);

            // --- SU kısmı ---
            if (once && !su_bitti) {
                // 7.1) Su tesisatçısı semaforuna bekle
                sem_wait(&su_tesisatcisi_sem);
                // 7.2) Dairenin mutex’ini al
                pthread_mutex_lock(&daire_kilitleri[kat_no-1][daire_no-1]);
                // 7.3) Boşta tesisatçı al
                int t_no = tesisatci_al();

                // 7.4) Su tesisatçısı giriş mesajı
                pthread_mutex_lock(&yazici);
                printf("  [Kat %d] Daire %d -> SU TESİSATÇISI [T%d] GİRİYOR 💧\n",
                       kat_no, daire_no, t_no+1);
                fflush(stdout);
                pthread_mutex_unlock(&yazici);

                // 7.5) İşlem süresi simülasyonu
                usleep(1500000 + rand_r(&seed) % 1000000);
                seed += rand_r(&seed);

                // 7.6) %10 olasılıkla su baskını senaryosu
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

                    // 7.7) Müdahale süresi
                    usleep(3000000);
                    sem_post(&su_cekici_sem);

                    // 7.8) Hata sonrası yeniden başlatma
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

                // 7.9) Normal su işi tamamlandı
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

                // %10 olasılıkla yangın senaryosu
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

                    // Yeniden başlatma
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

    // 8) Bitiş zamanını kaydet
    gettimeofday(&end_times[kat_no-1][daire_no-1], NULL);

    // 9) Aktif işçi sayısını azalt
    pthread_mutex_lock(&worker_kilit);
    active_workers--;
    pthread_mutex_unlock(&worker_kilit);

    return NULL;
}

/*
 * Kat inşaatını yöneten fonksiyon
 */
void kat_insa(int kat_no) {
    // 1) Thread dizisi ve dinamik daire numaraları dizisi oluştur
    pthread_t threads[DAIRE_SAYISI];
    int daireler[DAIRE_SAYISI];
    for (int j = 0; j < DAIRE_SAYISI; ++j)
        daireler[j] = j + 1;

    // 2) Daire sırasını rastgele karıştır
    karistir(daireler, DAIRE_SAYISI);

    // 3) Kat inşaat başlangıç mesajı
    pthread_mutex_lock(&yazici);
    printf("\n=== KAT %d İNŞAATI BAŞLADI (PID: %d) ===\n",
           kat_no, getpid());
    fflush(stdout);
    pthread_mutex_unlock(&yazici);

    // 4) Her daire için thread yarat
    for (int i = 0; i < DAIRE_SAYISI; i++) {
        int *bilgiler = malloc(2 * sizeof(int));
        bilgiler[0]   = kat_no;
        bilgiler[1]   = daireler[i];
        pthread_create(&threads[i], NULL, daire_insa, bilgiler);
    }

    // 5) Tüm thread’lerin bitmesini bekle
    for (int i = 0; i < DAIRE_SAYISI; i++)
        pthread_join(threads[i], NULL);

    // 6) Kat tamamlandı mesajı
    pthread_mutex_lock(&yazici);
    printf("=== KAT %d TAMAMLANDI ===\n", kat_no);
    fflush(stdout);
    pthread_mutex_unlock(&yazici);
}
