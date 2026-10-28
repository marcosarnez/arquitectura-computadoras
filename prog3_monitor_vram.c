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
        memset(sys, 0, sizeof(SystemArchitecture));
        for (int i = VRAM_INI; i <= VRAM_FIN; i++) sys->RAM[i] = 0x20;
        sys->cursor_vram = VRAM_INI;
    } else {
        fread(sys, sizeof(SystemArchitecture), 1, f);
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

// Experimento 3: Desplazamiento Vertical (Scroll) con LOAD y STORE
void scroll_vertical_vram(SystemArchitecture *sys) {
    for (int fila = 0; fila < 3; fila++) {
        for (int col = 0; col < 4; col++) {
            uint8_t dir_origen  = VRAM_INI + ((fila + 1) * 4) + col;
            uint8_t dir_destino = VRAM_INI + (fila * 4) + col;

            // LOAD R1, [dir_origen]
            sys->MAR = dir_origen; sys->MEM_READ = 1;
            sys->MDR = sys->RAM[sys->MAR];
            sys->MEM_READ = 0;

            // STORE R1, [dir_destino]
            sys->MAR = dir_destino; sys->MEM_WRITE = 1;
            sys->RAM[sys->MAR] = sys->MDR;
            sys->MEM_WRITE = 0;
        }
    }
    for (int col = 0; col < 4; col++) {
        sys->RAM[VRAM_INI + 12 + col] = 0x20; // Limpiar ultima fila (0xFC - 0xFF)
    }
    sys->cursor_vram = 0xFC;
    printf(" >> [SCROLL VRAM] Lineas desplazadas hacia arriba. Cursor ubicado en 0xFC.\n");
}

// Reto Integrador: Extraer de Cola FIFO -> Proyectar en VRAM con soporte de Backspace
void consumir_cola_hacia_vram(SystemArchitecture *sys) {
    if (sys->RAM[KB_COUNT] == 0) {
        printf(" >> [AVISO] La cola FIFO (0x70-0x77) esta vacia. Use ./prog1 para ingresar teclas.\n");
        return;
    }

    while (sys->RAM[KB_COUNT] > 0) {
        uint8_t head = sys->RAM[KB_HEAD];
        uint8_t dir_cola = COLA_BASE + head;

        // LOAD de la Cola FIFO
        sys->MAR = dir_cola; sys->MEM_READ = 1;
        uint8_t tecla = sys->RAM[sys->MAR];
        sys->MEM_READ = 0;

        sys->RAM[dir_cola] = 0x00;
        sys->RAM[KB_HEAD] = (head + 1) % COLA_TAM;
        sys->RAM[KB_COUNT]--;

        // Manejo de Backspace (ASCII 0x08)
        if (tecla == 0x08) {
            if (sys->cursor_vram > VRAM_INI) {
                sys->cursor_vram--;
                sys->MAR = sys->cursor_vram;
                sys->MDR = 0x20; // Espacio en blanco
                sys->MEM_WRITE = 1;
                sys->RAM[sys->MAR] = sys->MDR;
                sys->MEM_WRITE = 0;
                printf(" >> [BACKSPACE 0x08] Cursor retrocede a 0x%02X y escribe espacio (0x20).\n", sys->cursor_vram);
            }
        } else {
            if (sys->cursor_vram > VRAM_FIN) {
                scroll_vertical_vram(sys);
            }
            int fila = (sys->cursor_vram - VRAM_INI) / 4;
            int col  = (sys->cursor_vram - VRAM_INI) % 4;

            // STORE en VRAM: Dir = 0xF0 + (Fila * 4) + Columna
            sys->MAR = sys->cursor_vram;
            sys->MDR = tecla;
            sys->MEM_WRITE = 1;
            sys->RAM[sys->MAR] = sys->MDR;
            sys->MEM_WRITE = 0;

            printf(" >> [VRAM STORE] '%c' (0x%02X) -> Dir 0x%02X [Fila %d, Col %d]\n",
                   tecla, tecla, sys->MAR, fila, col);
            sys->cursor_vram++;
        }
    }
    guardar_sistema(sys);
}

void escribir_coordenada_directa(SystemArchitecture *sys) {
    int fila, col;
    char ch;
    printf("Ingrese Fila (0-3), Columna (0-3) y Caracter (ej: 2 1 X): ");
    if (scanf("%d %d %c", &fila, &col, &ch) == 3) {
        if (fila >= 0 && fila < 4 && col >= 0 && col < 4) {
            uint8_t dir = VRAM_INI + (fila * 4) + col;
            sys->MAR = dir; sys->MDR = (uint8_t)ch; sys->MEM_WRITE = 1;
            sys->RAM[sys->MAR] = sys->MDR;
            sys->MEM_WRITE = 0;
            guardar_sistema(sys);
            printf(" >> [STORE DIRECTO] 0xF0 + (%d*4) + %d = 0x%02X <- '%c'\n", fila, col, dir, ch);
        }
    }
}

void renderizar_matriz_vram(SystemArchitecture *sys) {
    printf("\n--- MONITOR VRAM 4x4 MAPEADO EN MEMORIA (0xF0 - 0xFF) ---\n");
    printf("Cursor actual: 0x%02X | Ultimo MAR: 0x%02X | Ultimo MDR: 0x%02X\n\n",
           sys->cursor_vram, sys->MAR, sys->MDR);
    for (int f = 0; f < 4; f++) {
        printf("  Fila %d: ", f);
        for (int c = 0; c < 4; c++) {
            uint8_t dir = VRAM_INI + (f * 4) + c;
            printf("[0x%02X: '%c'] ", dir, sys->RAM[dir]);
        }
        printf("\n");
    }
    printf("---------------------------------------------------------\n");
}

int main(void) {
    SystemArchitecture sys;
    cargar_sistema(&sys);

    int opcion;
    printf("=== PROGRAMA 3: CONTROLADOR DE MONITOR VRAM 4x4 (0xF0-0xFF) ===\n");
    printf(" [1] Consumir Cola FIFO (0x70-0x77) y proyectar en Monitor VRAM\n");
    printf(" [2] Escribir en coordenada (Fila, Columna) -> Dir = 0xF0 + (Fila*4) + Col\n");
    printf(" [3] Ejecutar Scroll Vertical en VRAM (Subir 1 linea)\n");
    printf(" [4] Solo mostrar estado actual del Monitor VRAM\n");
    printf("Seleccione opcion: ");
    if (scanf("%d", &opcion) != 1) return 1;

    if (opcion == 1) consumir_cola_hacia_vram(&sys);
    else if (opcion == 2) escribir_coordenada_directa(&sys);
    else if (opcion == 3) { scroll_vertical_vram(&sys); guardar_sistema(&sys); }

    renderizar_matriz_vram(&sys);
    return 0;
}
