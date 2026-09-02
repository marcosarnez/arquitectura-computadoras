#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_ELEMENTOS 1000
#define SUB_ELEMENTOS 100
#define INDICE_INICIO 500
#define ARCHIVO_DATOS "memoria_secundaria.bin"

// Función auxiliar para preparar el archivo en memoria secundaria si no existe
void preparar_memoria_secundaria(const char *nombre_archivo) {
    FILE *f = fopen(nombre_archivo, "rb");
    if (f == NULL) {
        printf("[INFO] Creando archivo '%s' en memoria secundaria (disco)...\n", nombre_archivo);
        f = fopen(nombre_archivo, "wb");
        if (!f) {
            perror("Error al crear el archivo");
            exit(1);
        }
        int buffer[TOTAL_ELEMENTOS];
        for (int i = 0; i < TOTAL_ELEMENTOS; i++) {
            buffer[i] = (i + 1) * 10; // Valores de prueba: 10, 20, 30, ...
        }
        fwrite(buffer, sizeof(int), TOTAL_ELEMENTOS, f);
        fclose(f);
    } else {
        fclose(f);
    }
}

int main() {
    // 0. Asegurar que existe el archivo en memoria secundaria
    preparar_memoria_secundaria(ARCHIVO_DATOS);

    // 1. Leer un arreglo de 1000 enteros almacenado en memoria secundaria hacia la RAM
    int arreglo_original[TOTAL_ELEMENTOS];

    FILE *archivo = fopen(ARCHIVO_DATOS, "rb");
    if (!archivo) {
        perror("Error al abrir memoria secundaria");
        return 1;
    }
    fread(arreglo_original, sizeof(int), TOTAL_ELEMENTOS, archivo);
    fclose(archivo);

    printf("=== PASO 1: LECTURA DESDE MEMORIA SECUNDARIA ===\n");
    printf("Primeros 5 elementos antes del traslado:\n  ");
    for (int i = 0; i < 5; i++) {
        printf("[%d]: %d  ", i, arreglo_original[i]);
    }
    printf("\nUltimos 5 elementos antes del traslado:\n  ");
    for (int i = TOTAL_ELEMENTOS - 5; i < TOTAL_ELEMENTOS; i++) {
        printf("[%d]: %d  ", i, arreglo_original[i]);
    }
    printf("\n\n");

    // 2. Declarar el segundo arreglo y transferir los 100 elementos (500 al 599)
    int segundo_arreglo[SUB_ELEMENTOS];

    // Transferencia eficiente a nivel de memoria usando memcpy
    memcpy(segundo_arreglo, &arreglo_original[INDICE_INICIO], SUB_ELEMENTOS * sizeof(int));

    // Limpiar (establecer a cero) las posiciones transferidas en el arreglo original
    memset(&arreglo_original[INDICE_INICIO], 0, SUB_ELEMENTOS * sizeof(int));

    // 3. Salidas por consola requeridas
    printf("=== PASO 2: ELEMENTOS TRANSFERIDOS AL SEGUNDO ARREGLO (100 elementos) ===\n");
    for (int i = 0; i < SUB_ELEMENTOS; i++) {
        printf("%6d ", segundo_arreglo[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\n");

    // Validación de que los índices 500 al 599 ahora son 0
    printf("=== PASO 3: VALIDACION DE CEROS EN ARREGLO ORIGINAL (500 a 599) ===\n");
    int todos_ceros = 1;
    for (int i = INDICE_INICIO; i < INDICE_INICIO + SUB_ELEMENTOS; i++) {
        if (arreglo_original[i] != 0) {
            todos_ceros = 0;
            break;
        }
    }

    if (todos_ceros) {
        printf("EXITO: Todos los indices del 500 al 599 contienen exactamente 0.\n");
    } else {
        printf("FALLO: Se encontraron valores distintos de 0 en el rango transferido.\n");
    }

    // Muestra de control en el rango 498 a 503 para ver la frontera
    printf("\nVista previa de frontera (indices 498 a 503):\n");
    for (int i = 498; i <= 503; i++) {
        printf("  arreglo_original[%d] = %d\n", i, arreglo_original[i]);
    }

    return 0;
}