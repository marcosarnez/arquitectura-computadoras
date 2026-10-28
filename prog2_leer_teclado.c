#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BUS_FILE   "/tmp/arch_system_bus.bin"
#define COLA_BASE  0x70
#define COLA_TAM   8
#define KB_HEAD    0x78
#define KB_TAIL    0x79
#define KB_COUNT   0x7A
#define VRAM_INI   0xF0
#define VRAM_FIN   0xFF

#pragma pack(push, 1)
typedef struct {
    uint8_t MAR;
    uint8_t MDR;
    uint8_t MEM_READ;
    uint8_t MEM_WRITE;
    uint8_t IO_READ;
    uint8_t IO_WRITE;
    uint8_t port_01_status;
    uint8_t port_02_data;
    uint8_t cursor_vram;
    uint8_t RAM[256];
} SystemArchitecture;
#pragma pack(pop)

void cargar_sistema(SystemArchitecture *sys) {
    FILE *f = fopen(BUS_FILE, "rb");
    if (!f) {
        printf(">> No existe memoria activa. Ejecute primero ./prog1\n");
        exit(1);
    }
    fread(sys, sizeof(SystemArchitecture), 1, f);
    fclose(f);
}

void guardar_sistema(const SystemArchitecture *sys) {
    FILE *f = fopen(BUS_FILE, "wb");
    if (f) {
        fwrite(sys, sizeof(SystemArchitecture), 1, f);
        fclose(f);
    }
}

// Experimento 1: Lectura por Puerto E/S Aislada (IN R1,[0x01] -> IN R2,[0x02])
void leer_desde_puerto_aislado(SystemArchitecture *sys) {
    printf("\n--- [OPCION 1: SONDEO Y LECTURA POR PUERTO (IN 0x01 / IN 0x02)] ---\n");
    printf("Ciclo IN R1, [0x01] -> MAR=0x01 | Bus Control: IO_READ=1, MEM_READ=0 (RAM 0x01/0x02 inhibida)\n");
    printf("Estado Puerto 0x01 (Status): %d\n", sys->port_01_status);

    if ((sys->port_01_status & 0x01) == 0) {
        printf(">> [PUERTO 0x02] No hay tecla pendiente en el registro del puerto (Status = 0).\n");
    } else {
        sys->MAR = 0x02;
        sys->IO_READ = 1; sys->MEM_READ = 0; sys->MEM_WRITE = 0; sys->IO_WRITE = 0;
        sys->MDR = sys->port_02_data;
        sys->port_01_status = 0x00; // El hardware limpia el Puerto 0x01 a 0 automaticamente
        sys->IO_READ = 0;
        guardar_sistema(sys);

        printf(">> [RESULTADO IN R2, [0x02]]: Dato en MDR = 0x%02X ('%c')\n",
               sys->MDR, (sys->MDR == 0x08 ? '<' : sys->MDR));
        printf(">> Handshake Hardware: Puerto 0x01 reseteado automaticamente a 0.\n");
    }
}

// Experimento 2: Extraccion de un elemento de la Cola Circular FIFO en RAM
void extraer_uno_de_cola_ram(SystemArchitecture *sys) {
    printf("\n--- [OPCION 2: EXTRACCION DIRECTA DE COLA FIFO EN RAM (0x70-0x77)] ---\n");
    if (sys->RAM[KB_COUNT] == 0) {
        printf(">> [COLA VACIA] KB_COUNT == 0 (HEAD=%d, TAIL=%d). No hay teclas para extraer.\n",
               sys->RAM[KB_HEAD], sys->RAM[KB_TAIL]);
        return;
    }

    uint8_t head = sys->RAM[KB_HEAD];
    uint8_t dir_lectura = COLA_BASE + head;

    // Micro-paso LOAD: MAR <- Dir, MEM_READ = 1, MDR <- RAM[MAR]
    sys->MAR = dir_lectura;
    sys->MEM_READ = 1; sys->IO_READ = 0;
    sys->MDR = sys->RAM[sys->MAR];
    sys->MEM_READ = 0;

    // Limpiar celda leida y avanzar KB_HEAD = (KB_HEAD + 1) mod 8, KB_COUNT--
    sys->RAM[dir_lectura] = 0x00;
    sys->RAM[KB_HEAD] = (head + 1) % COLA_TAM;
    sys->RAM[KB_COUNT]--;
    guardar_sistema(sys);

    printf(">> Ciclo LOAD -> MAR=0x%02X | MEM_READ=1 | MDR=0x%02X ('%c')\n",
           sys->MAR, sys->MDR, (sys->MDR == 0x08 ? '<' : sys->MDR));
    printf(">> Punteros actualizados -> HEAD=%d | TAIL=%d | COUNT=%d/8\n",
           sys->RAM[KB_HEAD], sys->RAM[KB_TAIL], sys->RAM[KB_COUNT]);
}

void inspeccionar_cola_completa(SystemArchitecture *sys) {
    printf("\n--- [OPCION 3: INSPECCION DE REGISTROS Y COLA CIRCULAR EN RAM] ---\n");
    printf("Registros CPU -> MAR: 0x%02X | MDR: 0x%02X\n", sys->MAR, sys->MDR);
    printf("Puertos E/S   -> [0x01 Status]: %d | [0x02 Data]: 0x%02X\n",
           sys->port_01_status, sys->port_02_data);
    printf("Punteros Cola -> [0x78 HEAD]: %d   | [0x79 TAIL]: %d   | [0x7A COUNT]: %d/8\n",
           sys->RAM[KB_HEAD], sys->RAM[KB_TAIL], sys->RAM[KB_COUNT]);
    printf("Celdas FIFO   -> ");
    for (int i = 0; i < COLA_TAM; i++) {
        uint8_t v = sys->RAM[COLA_BASE + i];
        char c = (v >= 32 && v <= 126) ? v : (v == 0x08 ? '<' : '.');
        printf("[0x%02X:%c] ", COLA_BASE + i, c);
    }
    printf("\n");
}

int main(void) {
    SystemArchitecture sys;
    cargar_sistema(&sys);

    int opcion;
    printf("=== PROGRAMA 2: LECTOR DE PUERTOS E/S Y COLA FIFO DE TECLADO ===\n");
    printf(" [1] Leer desde Puerto de Hardware (IN 0x01 / IN 0x02 con IO_READ=1)\n");
    printf(" [2] Extraer 1 caracter de la Cola FIFO en RAM (0x70 + KB_HEAD)\n");
    printf(" [3] Inspeccionar estado actual de Registros (MAR/MDR) y Cola RAM\n");
    printf("Seleccione opcion: ");
    if (scanf("%d", &opcion) != 1) return 1;

    if (opcion == 1) leer_desde_puerto_aislado(&sys);
    else if (opcion == 2) extraer_uno_de_cola_ram(&sys);
    else if (opcion == 3) inspeccionar_cola_completa(&sys);

    return 0;
}
