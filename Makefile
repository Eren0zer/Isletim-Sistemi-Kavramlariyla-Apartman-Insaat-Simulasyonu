CC = gcc
CFLAGS = -Wall -pthread

all: apartman

apartman: main.o bina.o daire.o
	$(CC) $(CFLAGS) -o apartman main.o bina.o daire.o

main.o: main.c bina.h
	$(CC) $(CFLAGS) -c main.c

bina.o: bina.c bina.h
	$(CC) $(CFLAGS) -c bina.c

daire.o: daire.c bina.h
	$(CC) $(CFLAGS) -c daire.c

clean:
	rm -f *.o apartman log.txt report.txt insa_suresi.csv

# Test çıktıları için dizin
RESULTS_DIR := results

# tests öncesi results klasörünü hazırla
.PHONY: prepare_results
prepare_results:
	mkdir -p $(RESULTS_DIR)

# Özet testi
repeated: prepare_results apartman
	chmod +x scripts/repeated.sh
	RESULTS_DIR=$(RESULTS_DIR) ./scripts/repeated.sh

# Parametre sweep testi
param: prepare_results apartman
	chmod +x scripts/param_sweep.sh
	RESULTS_DIR=$(RESULTS_DIR) ./scripts/param_sweep.sh

# Hata oranı sweep testi
error: prepare_results apartman
	chmod +x scripts/error_sweep.sh
	RESULTS_DIR=$(RESULTS_DIR) ./scripts/error_sweep.sh

# Ölçek testi
scale: prepare_results apartman
	chmod +x scripts/scale_sweep.sh
	RESULTS_DIR=$(RESULTS_DIR) ./scripts/scale_sweep.sh

# Profil testi
profile: prepare_results apartman
	chmod +x scripts/profile.sh
	RESULTS_DIR=$(RESULTS_DIR) ./scripts/profile.sh
