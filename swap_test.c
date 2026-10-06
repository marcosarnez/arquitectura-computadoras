#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK_SIZE_MB 50 // Asigna 50 MB por iteración

int main() {
    size_t chunk_bytes = CHUNK_SIZE_MB * 1024 * 1024;
    size_t total_mb = 0;

    printf("[+] Iniciando asignacion de memoria...\n");

    while (1) {
        // 1. Solicitar bloque de memoria
        char *buffer = (char *)malloc(chunk_bytes);
        if (buffer == NULL) {
            printf("\n[!] Error: malloc no pudo reservar mas memoria virtual.\n");
            break;
        }

        // 2. Tocar la memoria para forzar la asignacion fisica en RAM
        memset(buffer, 0xAA, chunk_bytes);

        total_mb += CHUNK_SIZE_MB;
        printf("\r[+] Memoria asignada y tocada: %zu MB", total_mb);
        fflush(stdout);

        // Pequeña pausa para permitir monitorear el consumo de Swap
        usleep(50000); // 50 milisegundos
    }

    printf("\n[+] Presiona ENTER para liberar y salir...");
    getchar();
    return 0;
}
