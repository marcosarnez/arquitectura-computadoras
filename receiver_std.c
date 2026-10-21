#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define FIFO_PATH "/tmp/ipc_sensor_fifo"
#define TOTAL_MSG 5000000
#define MSG_SIZE 128

typedef struct {
    int seq_num;
    int sensor_id;
    char telemetry_data[MSG_SIZE - (2 * sizeof(int))];
} SensorFrame;

int main() {
    mkfifo(FIFO_PATH, 0666);
    printf("[Receiver STD] Conectando al canal FIFO...\n");
    
    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir FIFO");
        return 1;
    }

    SensorFrame frame;
    size_t count = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (count < TOTAL_MSG) {
        if (read(fd, &frame, sizeof(SensorFrame)) > 0) {
            count++;
        }
    }

    close(fd);
    unlink(FIFO_PATH);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double mps = count / elapsed;
    double mbps = (double)(count * MSG_SIZE) / (1024.0 * 1024.0 * elapsed);

    printf("\n=== METRICAS RECEPTOR ESTANDAR (receiver_std) ===\n");
    printf("Tramas recibidas  : %zu\n", count);
    printf("Tamano de trama   : %d bytes\n", MSG_SIZE);
    printf("Tiempo total      : %.4f segundos\n", elapsed);
    printf("Velocidad (MPS)   : %.0f tramas/seg\n", mps);
    printf("Ancho de banda    : %.2f MB/s\n", mbps);

    return 0;
}
