#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("[Sender STD] Esperando conexion del receptor en el FIFO...\n");
    
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir FIFO");
        return 1;
    }

    SensorFrame frame;
    frame.sensor_id = 101;
    memset(frame.telemetry_data, 'S', sizeof(frame.telemetry_data));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < TOTAL_MSG; i++) {
        frame.seq_num = i;
        if (write(fd, &frame, sizeof(SensorFrame)) <= 0) {
            perror("Error en syscall write");
            break;
        }
    }

    close(fd);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double mps = TOTAL_MSG / elapsed;
    double mbps = (double)(TOTAL_MSG * MSG_SIZE) / (1024.0 * 1024.0 * elapsed);

    printf("\n=== METRICAS EMISOR ESTANDAR (sender_std) ===\n");
    printf("Tramas enviadas   : %d\n", TOTAL_MSG);
    printf("Tamano de trama   : %d bytes\n", MSG_SIZE);
    printf("Tiempo total      : %.4f segundos\n", elapsed);
    printf("Velocidad (MPS)   : %.0f tramas/seg\n", mps);
    printf("Ancho de banda    : %.2f MB/s\n", mbps);

    return 0;
}
