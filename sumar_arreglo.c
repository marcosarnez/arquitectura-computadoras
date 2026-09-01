#include <stdio.h>

int main() {
    int arreglo[] = {5, 10, 15, 20, 25};
    int n = sizeof(arreglo) / sizeof(arreglo[0]);
    int suma = 0;

    __asm__ (
        "movl $0, %%eax\n\t"        // EAX = acumulador (suma = 0)
        "movl $0, %%ecx\n\t"        // ECX = índice i (i = 0)

        "1:\n\t"
        "cmpl %2, %%ecx\n\t"        // Comparar índice con n
        "jge 2f\n\t"                // Si i >= n, salir del ciclo
        "addl (%1, %%rcx, 4), %%eax\n\t" // EAX += arreglo[i] (desplazamiento base + i*4 bytes)
        "incl %%ecx\n\t"            // i++
        "jmp 1b\n\t"                // Volver a evaluar el ciclo

        "2:\n\t"
        "movl %%eax, %0\n\t"        // Guardar resultado de EAX en la variable 'suma'
        : "=r" (suma)               // Salida: %0
        : "r" (arreglo), "r" (n)    // Entradas: %1 (puntero al arreglo), %2 (longitud n)
        : "eax", "ecx", "cc", "memory" // Registros modificados y memoria
    );

    printf("La suma total del arreglo es: %d\n", suma);

    return 0;
}