#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define SHM_NAME "/ipc_sensor_shm"
#define TOTAL_MSG 5000000
#define MSG_SIZE 128
#define BATCH_SIZE 1000
#define NUM_BATCHES (TOTAL_MSG / BATCH_SIZE)

typedef struct {
    int seq_num;
    int sensor_id;
    char telemetry_data[MSG_SIZE - (2 * sizeof(int))];
} SensorFrame;

typedef struct {
    sem_t sem_data_ready;
    sem_t sem_buffer_free;
    SensorFrame batch_buffer[BATCH_SIZE];
} SharedMemoryBus;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("[Receiver OPT] Error al abrir memoria compartida (ejecuta sender_opt primero)");
        return 1;
    }

    SharedMemoryBus *bus = mmap(0, sizeof(SharedMemoryBus), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    printf("[Receiver OPT] Leyendo lotes directamente desde Memoria Compartida...\n");

    size_t count = 0;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int b = 0; b < NUM_BATCHES; b++) {
        sem_wait(&bus->sem_data_ready);
        count += BATCH_SIZE;
        sem_post(&bus->sem_buffer_free);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double mps = count / elapsed;
    double mbps = (double)(count * MSG_SIZE) / (1024.0 * 1024.0 * elapsed);

    printf("\n=== METRICAS RECEPTOR OPTIMIZADO (receiver_opt) ===\n");
    printf("Tramas recibidas  : %zu\n", count);
    printf("Tamano de lote    : %d tramas\n", BATCH_SIZE);
    printf("Tiempo total      : %.4f segundos\n", elapsed);
    printf("Velocidad (MPS)   : %.0f tramas/seg\n", mps);
    printf("Ancho de banda    : %.2f MB/s\n", mbps);

    sem_destroy(&bus->sem_data_ready);
    sem_destroy(&bus->sem_buffer_free);
    munmap(bus, sizeof(SharedMemoryBus));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return 0;
}
