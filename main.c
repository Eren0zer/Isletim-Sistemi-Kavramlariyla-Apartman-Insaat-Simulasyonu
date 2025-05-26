#include "bina.h"
#include <float.h>      // DBL_MAX için
#include <sys/mman.h>   // mmap, MAP_SHARED, MAP_ANONYMOUS
#include <fcntl.h>      // (opsiyonel) O_* makroları
#include <unistd.h>     // fork, usleep, etc.

int main() {
    // 1️⃣ Paylaşılan bellek ile start/end zaman dizilerini ayır
    size_t sz = sizeof(struct timeval) * KAT_SAYISI * DAIRE_SAYISI;
    start_times = mmap(
        NULL, sz,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1, 0
    );
    end_times = mmap(
        NULL, sz,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1, 0
    );
    if (start_times == MAP_FAILED || end_times == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    // Başlangıçta sıfırla
    memset(start_times, 0, sz);
    memset(end_times,   0, sz);

    // 2️⃣ Log dosyasını aç
    log_fp = fopen("log.txt", "w");
    if (!log_fp) {
        perror("log.txt açılamadı");
        exit(EXIT_FAILURE);
    }

    // 3️⃣ Rastgele tohum ve daire durumlarını sıfırla
    srand(time(NULL));
    memset(daire_durumlari, 0, sizeof(daire_durumlari));

    // 4️⃣ Semaforları başlat
    sem_init(&su_tesisatcisi_sem, 0, TESISATCI_SAYISI);
    sem_init(&elektrikci_sem,     0, ELEKTRIKCI_SAYISI);
    sem_init(&itfaiyeci_sem,      0, 1);
    sem_init(&su_cekici_sem,      0, 1);

    // 5️⃣ Daire kilit mutex’lerini initialize et
    for (int i = 0; i < KAT_SAYISI; i++)
        for (int j = 0; j < DAIRE_SAYISI; j++)
            pthread_mutex_init(&daire_kilitleri[i][j], NULL);

    // 6️⃣ Kat inşaatlarını fork + wait ile sırayla yap
    for (int kat = 1; kat <= KAT_SAYISI; kat++) {
        pid_t pid = fork();
        if (pid == 0) {
            kat_insa(kat);
            exit(0);
        } else {
            wait(NULL);
        }
    }

    printf("\n🏢 TÜM APARTMAN BAŞARIYLA İNŞA EDİLDİ 🏁\n");

    // 7️⃣ Sonuç özeti hesaplama
    struct timeval sim_start = start_times[0][0];
    struct timeval sim_end   = end_times  [0][0];
    for (int i = 0; i < KAT_SAYISI; i++) {
        for (int j = 0; j < DAIRE_SAYISI; j++) {
            if (timercmp(&start_times[i][j], &sim_start, <))
                sim_start = start_times[i][j];
            if (timercmp(&end_times  [i][j], &sim_end,   >))
                sim_end   = end_times  [i][j];
        }
    }
    double total = (sim_end.tv_sec  - sim_start.tv_sec) +
                   (sim_end.tv_usec - sim_start.tv_usec) / 1e6;

    double fastest = DBL_MAX, slowest = 0;
    int fast_k = 0, fast_d = 0, slow_k = 0, slow_d = 0;
    double floor_sum[KAT_SAYISI] = {0};

    for (int i = 0; i < KAT_SAYISI; i++) {
        for (int j = 0; j < DAIRE_SAYISI; j++) {
            double dur = (end_times[i][j].tv_sec  - start_times[i][j].tv_sec) +
                         (end_times[i][j].tv_usec - start_times[i][j].tv_usec) / 1e6;
            if (dur < fastest) {
                fastest = dur; fast_k = i; fast_d = j;
            }
            if (dur > slowest) {
                slowest = dur; slow_k = i; slow_d = j;
            }
            floor_sum[i] += dur;
        }
    }

    // 8️⃣ Ekrana yazdırma
    printf("\n--- Sonuç Özeti ---\n");
    printf("Simülasyon toplam süresi : %.3f saniye\n", total);
    printf("En hızlı daire           : Kat %d, Daire %d (%.3f s)\n",
           fast_k+1, fast_d+1, fastest);
    printf("En yavaş daire           : Kat %d, Daire %d (%.3f s)\n",
           slow_k+1, slow_d+1, slowest);
    for (int i = 0; i < KAT_SAYISI; i++) {
        double avg = floor_sum[i] / DAIRE_SAYISI;
        printf("Kat %d ortalama süre     : %.3f saniye\n", i+1, avg);
    }

    // 9️⃣ report.txt’e yaz
    FILE *report = fopen("report.txt", "w");
    if (report) {
        fprintf(report, "--- Sonuç Özeti ---\n");
        fprintf(report, "Simülasyon toplam süresi : %.3f saniye\n", total);
        fprintf(report, "En hızlı daire           : Kat %d, Daire %d (%.3f s)\n",
                fast_k+1, fast_d+1, fastest);
        fprintf(report, "En yavaş daire           : Kat %d, Daire %d (%.3f s)\n",
                slow_k+1, slow_d+1, slowest);
        for (int i = 0; i < KAT_SAYISI; i++) {
            double avg = floor_sum[i] / DAIRE_SAYISI;
            fprintf(report, "Kat %d ortalama süre     : %.3f saniye\n", i+1, avg);
        }
        fclose(report);
    }

    // 10️⃣ insa_suresi.csv’e CSV formatında yaz
    FILE *csv = fopen("insa_suresi.csv", "w");
    if (csv) {
        fprintf(csv, "Kat,Daire,Start_s,Start_us,End_s,End_us,Duration_s\n");
        for (int i = 0; i < KAT_SAYISI; i++) {
            for (int j = 0; j < DAIRE_SAYISI; j++) {
                double dur = (end_times[i][j].tv_sec  - start_times[i][j].tv_sec) +
                             (end_times[i][j].tv_usec - start_times[i][j].tv_usec) / 1e6;
                fprintf(csv, "%d,%d,%ld,%ld,%ld,%ld,%.6f\n",
                        i+1, j+1,
                        (long)start_times[i][j].tv_sec,
                        (long)start_times[i][j].tv_usec,
                        (long)end_times  [i][j].tv_sec,
                        (long)end_times  [i][j].tv_usec,
                        dur);
            }
        }
        fclose(csv);
    }

    // 11️⃣ Temizlik
    sem_destroy(&su_tesisatcisi_sem);
    sem_destroy(&elektrikci_sem);
    sem_destroy(&itfaiyeci_sem);
    sem_destroy(&su_cekici_sem);
    for (int i = 0; i < KAT_SAYISI; i++)
        for (int j = 0; j < DAIRE_SAYISI; j++)
            pthread_mutex_destroy(&daire_kilitleri[i][j]);

    fclose(log_fp);
    return 0;
}

