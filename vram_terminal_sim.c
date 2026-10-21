#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Memoria principal (0x00 a 0xFF) y Puertos de E/S
uint8_t RAM[256];
uint8_t puerto_estado = 0; // Puerto 0x01
uint8_t puerto_datos  = 0; // Puerto 0x02

// Direcciones del mapa de memoria
#define COLA_BASE  0x70
#define COLA_TAM   8
#define KB_HEAD    0x78
#define KB_TAIL    0x79
#define KB_COUNT   0x7A
#define VRAM_INI   0xF0
#define VRAM_FIN   0xFF

uint8_t cursor = VRAM_INI;

// 1. Lectura por puerto (E/S aislada)
uint8_t leer_puerto(uint8_t num_puerto) {
    if (num_puerto == 0x01) {
        return puerto_estado;
    }
    if (num_puerto == 0x02) {
        uint8_t tecla = puerto_datos;
        puerto_estado = 0; // Limpia el estado automaticamente al leer
        printf("[PUERTO] IN 0x02 (IO_READ=1) -> Tecla: 0x%02X | Puerto 0x01 reseteado a 0\n", tecla);
        return tecla;
    }
    return 0;
}

// 2. Productor: del puerto de teclado a la cola FIFO en RAM
void presionar_tecla(char c) {
    puerto_datos = (uint8_t)c;
    puerto_estado = 1;

    // Sondeo de puerto 0x01
    if (leer_puerto(0x01) == 1) {
        uint8_t dato = leer_puerto(0x02);

        if (RAM[KB_COUNT] == COLA_TAM) {
            printf("[COLA] Overflow! Cola llena (COUNT=8), se descarta '%c'\n", c);
            return;
        }

        uint8_t tail = RAM[KB_TAIL];
        RAM[COLA_BASE + tail] = dato;
        RAM[KB_TAIL] = (tail + 1) % COLA_TAM;
        RAM[KB_COUNT]++;
    }
}

// 3. Desplazamiento vertical (Scroll) en VRAM
void hacer_scroll() {
    for (int f = 0; f < 3; f++) {
        for (int c = 0; c < 4; c++) {
            RAM[VRAM_INI + (f * 4) + c] = RAM[VRAM_INI + ((f + 1) * 4) + c];
        }
    }
    for (int c = 0; c < 4; c++) {
        RAM[VRAM_INI + 12 + c] = 0x20; // Limpia ultima fila con espacios
    }
    cursor = 0xFC;
    printf("[VRAM] Scroll vertical aplicado. Cursor en 0xFC\n");
}

// 4. Consumidor: extrae de la cola FIFO y escribe en VRAM
void procesar_cola_a_vram() {
    if (RAM[KB_COUNT] == 0) {
        printf("[COLA] Vacia, no hay datos para enviar a VRAM.\n");
        return;
    }

    uint8_t head = RAM[KB_HEAD];
    uint8_t tecla = RAM[COLA_BASE + head];
    RAM[KB_HEAD] = (head + 1) % COLA_TAM;
    RAM[KB_COUNT]--;

    // Manejo de Backspace (0x08)
    if (tecla == 0x08) {
        if (cursor > VRAM_INI) {
            cursor--;
            RAM[cursor] = 0x20; // Borra con espacio en blanco
            printf("[VRAM] Backspace (0x08) -> Borrado en 0x%02X\n", cursor);
        }
        return;
    }

    if (cursor > VRAM_FIN) {
        hacer_scroll();
    }

    RAM[cursor] = tecla;
    int fila = (cursor - VRAM_INI) / 4;
    int col  = (cursor - VRAM_INI) % 4;
    printf("[VRAM] STORE '%c' en 0x%02X (Fila %d, Col %d)\n", tecla, cursor, fila, col);
    cursor++;
}

// Mostrar estado compacto de la arquitectura
void mostrar_estado() {
    printf("\n--- ESTADO DE MEMORIA Y REGISTROS ---\n");
    printf("Puertos  -> [0x01 Status]: %d | [0x02 Data]: 0x%02X\n", puerto_estado, puerto_datos);
    printf("Punteros -> [0x78 HEAD]: %d   | [0x79 TAIL]: %d   | [0x7A COUNT]: %d/8\n",
           RAM[KB_HEAD], RAM[KB_TAIL], RAM[KB_COUNT]);

    printf("Cola FIFO (0x70-0x77): ");
    for (int i = 0; i < COLA_TAM; i++) {
        uint8_t v = RAM[COLA_BASE + i];
        char ch = (v >= 32 && v <= 126) ? v : (v == 0x08 ? '<' : '.');
        printf("[%c] ", ch);
    }

    printf("\n\nMonitor VRAM 4x4 (0xF0-0xFF) | Cursor: 0x%02X\n", cursor);
    for (int f = 0; f < 4; f++) {
        printf("  F%d: ", f);
        for (int c = 0; c < 4; c++) {
            uint8_t dir = VRAM_INI + (f * 4) + c;
            printf("[0x%02X: %c] ", dir, RAM[dir]);
        }
        printf("\n");
    }
    printf("-------------------------------------\n");
}

int main() {
    memset(RAM, 0, sizeof(RAM));
    for (int i = VRAM_INI; i <= VRAM_FIN; i++) {
        RAM[i] = 0x20; // Inicializar pantalla con espacios
    }

    int op;
    char texto[64];

    while (1) {
        mostrar_estado();
        printf("1. Ingresar teclas  2. Backspace (0x08)  3. Extraer 1 a VRAM  4. Vaciar cola a VRAM  5. Scroll  0. Salir\n");
        printf("Opcion: ");
        if (scanf("%d", &op) != 1 || op == 0) break;
        getchar();

        if (op == 1) {
            printf("Texto a enviar: ");
            if (fgets(texto, sizeof(texto), stdin)) {
                texto[strcspn(texto, "\n")] = 0;
                for (int i = 0; texto[i] != '\0'; i++) {
                    presionar_tecla(texto[i]);
                }
            }
        } else if (op == 2) {
            presionar_tecla(0x08);
        } else if (op == 3) {
            procesar_cola_a_vram();
        } else if (op == 4) {
            while (RAM[KB_COUNT] > 0) procesar_cola_a_vram();
        } else if (op == 5) {
            hacer_scroll();
        }
    }
    return 0;
}
