#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Función para sumar y capturar Carry Flag (CF) y Overflow Flag (OF)
void sumar_y_verificar_banderas(uint8_t a, uint8_t b) {
    uint8_t resultado;
    unsigned char carry_flag;
    unsigned char overflow_flag;

    __asm__ (
        "addb %[val_b], %[val_a]\n\t"  // val_a = val_a + val_b (suma de 8 bits)
        "setc %[cf]\n\t"               // cf = 1 si Carry Flag (CF) activa, 0 si no
        "seto %[of]\n\t"               // of = 1 si Overflow Flag (OF) activa, 0 si no
        : [val_a] "+r" (a),
          [cf] "=q" (carry_flag),
          [of] "=q" (overflow_flag)
        : [val_b] "r" (b)
        : "cc"
    );

    resultado = a;
    printf("Operacion: %u + %u = %u (interpretado sin signo)\n", (unsigned int)a, (unsigned int)b, (unsigned int)resultado);
    printf("Operacion: %d + %d = %d (interpretado con signo int8)\n", (int8_t)a, (int8_t)b, (int8_t)resultado);
    printf("-> Carry Flag (CF - sin signo):    %s\n", carry_flag ? "ACTIVA (1)" : "INACTIVA (0)");
    printf("-> Overflow Flag (OF - con signo): %s\n\n", overflow_flag ? "ACTIVA (1)" : "INACTIVA (0)");
}

int main(void) {
    printf("=== Caso 1: Desbordamiento CON signo (OF) ===\n");
    // 100 + 50 = 150 (cabe en uint8 [0..255], pero excede int8 [-128..127])
    sumar_y_verificar_banderas(100, 50);

    printf("=== Caso 2: Acarreo/Desbordamiento SIN signo (CF) ===\n");
    // 200 + 100 = 300 (excede uint8 [0..255], se desborda a 44)
    sumar_y_verificar_banderas(200, 100);

    printf("=== Caso 3: Operacion Normal (Sin banderas) ===\n");
    sumar_y_verificar_banderas(10, 20);

    return 0;
}


