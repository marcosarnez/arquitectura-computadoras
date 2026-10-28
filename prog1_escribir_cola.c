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
    uint8_t MAR;            // Memory Address Register (Bus de Direcciones)
    uint8_t MDR;            // Memory Data Register (Bus de Datos)
    uint8_t MEM_READ;       // Linea de control Lectura RAM
    uint8_t MEM_WRITE;      // Linea de control Escritura RAM
    uint8_t IO_READ;        // Linea de control Lectura Puerto
    uint8_t IO_WRITE;       // Linea de control Escritura Puerto
    uint8_t port_01_status; // Puerto 0x01: Bit 0 = 1 si hay tecla lista
    uint8_t port_02_data;   // Puerto 0x02: Codigo ASCII de la tecla
    uint8_t cursor_vram;    // Puntero de cursor en VRAM (0xF0 - 0xFF)
    uint8_t RAM[256];       // Mapa de Memoria 0x00 a 0xFF
} SystemArchitecture;
#pragma pack(pop)

void cargar_sistema(SystemArchitecture *sys) {
    FILE *f = fopen(BUS_FILE, "rb");
    if (!f) {
        memset(sys, 0, sizeof(SystemArchitecture));
        for (int i = VRAM_INI; i <= VRAM_FIN; i++) sys->RAM[i] = 0x20;
        sys->cursor_vram = VRAM_INI;
    } else {
        if (fread(sys, sizeof(SystemArchitecture), 1, f) != 1) {
            memset(sys, 0, sizeof(SystemArchitecture));
        }
        fclose(f);
    }
}

void guardar_sistema(const SystemArchitecture *sys) {
    FILE *f = fopen(BUS_FILE, "wb");
    if (f) {
        fwrite(sys, sizeof(SystemArchitecture), 1, f);
        fclose(f);
    }
}

void validar_colision_bus(SystemArchitecture *sys) {
    int suma = sys->MEM_READ + sys->MEM_WRITE + sys->IO_READ + sys->IO_WRITE;
    if (suma > 1) {
        printf("[EXCEPCION] Conflicto de Bus detectado! Suma de lineas = %d\n", suma);
        exit(1);
    }
}

void pinchar_tecla_en_hardware(SystemArchitecture *sys, uint8_t ascii) {
    // 1. Escritura en Puerto E/S (OUT 0x02) -> Activa IO_WRITE=1, RAM inhibida
    sys->MAR = 0x02;
    sys->MDR = ascii;
    sys->IO_WRITE = 1; sys->MEM_WRITE = 0; sys->MEM_READ = 0; sys->IO_READ = 0;
    validar_colision_bus(sys);

    sys->port_02_data = sys->MDR;
    sys->port_01_status = 0x01; // Tecla disponible en el controlador
    sys->IO_WRITE = 0;

    // 2. Verificar Overflow en Cola Circular FIFO (0x70 - 0x77)
    if (sys->RAM[KB_COUNT] == COLA_TAM) {
        printf(" >> [OVERFLOW] KB_COUNT == 8. Tecla '%c' (0x%02X) descartada para proteger la cola.\n",
               (ascii == 0x08 ? '<' : ascii), ascii);
        guardar_sistema(sys);
        return;
    }

    // 3. Escritura en Cola RAM (STORE en 0x70 + KB_TAIL) -> Activa MEM_WRITE=1
    uint8_t tail = sys->RAM[KB_TAIL];
    uint8_t dir_cola = COLA_BASE + tail;

    sys->MAR = dir_cola;
    sys->MDR = ascii;
    sys->MEM_WRITE = 1;
    validar_colision_bus(sys);

    sys->RAM[sys->MAR] = sys->MDR;
    sys->MEM_WRITE = 0;

    // Aritmetica modular: KB_TAIL = (KB_TAIL + 1) mod 8 y KB_COUNT++
    sys->RAM[KB_TAIL] = (tail + 1) % COLA_TAM;
    sys->RAM[KB_COUNT]++;

    printf(" >> [OK] Puerto 0x02=0x%02X (Status 0x01=1) | MAR=0x%02X, MDR=0x%02X (MEM_WRITE=1) | TAIL=%d, COUNT=%d/8\n",
           ascii, sys->MAR, sys->MDR, sys->RAM[KB_TAIL], sys->RAM[KB_COUNT]);

    guardar_sistema(sys);
}

void mostrar_resumen_cola(SystemArchitecture *sys) {
    printf("\n--- ESTADO ACTUAL DE PUERTOS Y COLA FIFO (0x70-0x77) ---\n");
    printf("Puertos  -> [0x01 Status]: %d | [0x02 Data]: 0x%02X\n", sys->port_01_status, sys->port_02_data);
    printf("Punteros -> [0x78 HEAD]: %d   | [0x79 TAIL]: %d   | [0x7A COUNT]: %d/8\n",
           sys->RAM[KB_HEAD], sys->RAM[KB_TAIL], sys->RAM[KB_COUNT]);
    printf("Buffer   -> ");
    for (int i = 0; i < COLA_TAM; i++) {
        uint8_t v = sys->RAM[COLA_BASE + i];
        char c = (v >= 32 && v <= 126) ? v : (v == 0x08 ? '<' : '.');
        printf("[0x%02X:%c] ", COLA_BASE + i, c);
    }
    printf("\n--------------------------------------------------------\n");
}

int main(void) {
    SystemArchitecture sys;
    cargar_sistema(&sys);

    int opcion;
    char texto[64];

    printf("=== PROGRAMA 1: ESCRITOR EN PUERTO TECLADO (0x01/0x02) Y COLA FIFO ===\n");
    printf(" [1] Enviar palabra/rafaga de teclas a la Cola FIFO\n");
    printf(" [2] Enviar tecla BACKSPACE (ASCII 0x08) a la Cola FIFO\n");
    printf(" [3] Reiniciar toda la memoria del sistema (Reset)\n");
    printf("Seleccione opcion: ");
    if (scanf("%d", &opcion) != 1) return 1;
    getchar();

    if (opcion == 1) {
        printf("Ingrese caracteres a pinchar en el teclado: ");
        if (fgets(texto, sizeof(texto), stdin)) {
            texto[strcspn(texto, "\n")] = 0;
            for (int i = 0; texto[i] != '\0'; i++) {
                pinchar_tecla_en_hardware(&sys, (uint8_t)texto[i]);
            }
        }
    } else if (opcion == 2) {
        pinchar_tecla_en_hardware(&sys, 0x08);
    } else if (opcion == 3) {
        memset(&sys, 0, sizeof(SystemArchitecture));
        for (int i = VRAM_INI; i <= VRAM_FIN; i++) sys.RAM[i] = 0x20;
        sys.cursor_vram = VRAM_INI;
        guardar_sistema(&sys);
        printf(" >> Sistema reiniciado correctamente.\n");
    }

    mostrar_resumen_cola(&sys);
    return 0;
}
