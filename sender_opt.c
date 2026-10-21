#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error en shm_open");
        return 1;
    }

    ftruncate(shm_fd, sizeof(SharedMemoryBus));
    SharedMemoryBus *bus = mmap(0, sizeof(SharedMemoryBus), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_init(&bus->sem_data_ready, 1, 0);
    sem_init(&bus->sem_buffer_free, 1, 1);

    printf("[Sender OPT] Iniciando transmision Zero-Copy por lotes en RAM...\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int b = 0; b < NUM_BATCHES; b++) {
        sem_wait(&bus->sem_buffer_free);
        
        for (int i = 0; i < BATCH_SIZE; i++) {
            bus->batch_buffer[i].seq_num = (b * BATCH_SIZE) + i;
            bus->batch_buffer[i].sensor_id = 202;
            memset(bus->batch_buffer[i].telemetry_data, 'Z', sizeof(bus->batch_buffer[i].telemetry_data));
        }
        
        sem_post(&bus->sem_data_ready);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double mps = TOTAL_MSG / elapsed;
    double mbps = (double)(TOTAL_MSG * MSG_SIZE) / (1024.0 * 1024.0 * elapsed);

    printf("\n=== METRICAS EMISOR OPTIMIZADO (sender_opt) ===\n");
    printf("Tramas enviadas   : %d\n", TOTAL_MSG);
    printf("Tamano de lote    : %d tramas\n", BATCH_SIZE);
    printf("Tiempo total      : %.4f segundos\n", elapsed);
    printf("Velocidad (MPS)   : %.0f tramas/seg\n", mps);
    printf("Ancho de banda    : %.2f MB/s\n", mbps);

    munmap(bus, sizeof(SharedMemoryBus));
    close(shm_fd);
    return 0;
}
