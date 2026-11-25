#include <stdio.h>
#include <string.h>
#include "simulador.h"

int main(int argc, char **argv)
{
    int reto = argc == 2 && strcmp(argv[1], "--sin-x-ancilla") == 0;
    if (argc > 1 && !reto) {
        fprintf(stderr, "Uso: %s [--sin-x-ancilla]\n", argv[0]);
        return 1;
    }

    const int shots = 1000;
    const char *nombres[] = {"f(x) = 0", "f(x) = 1", "f(x) = x", "f(x) = 1 XOR x"};
    srand(42);
    printf("LAB 3 | Deutsch (%d mediciones por oraculo)\n", shots);
    printf("Ancilla tras H: %s | Una consulta por circuito\n", reto ? "|+>" : "|->");

    for (unsigned f = 0; f < 4; ++f) {
        double estado[4] = {1, 0, 0, 0};
        if (!reto)
            x(estado, 2, 1, 0);
        h(estado, 2, 0);
        h(estado, 2, 1);

        /* Una aplicación de U_f: |x,y> pasa a |x,y XOR f(x)>. */
        if (f & 1u)
            x(estado, 2, 1, 0);
        if (f & 2u)
            x(estado, 2, 1, 1u << 0);

        h(estado, 2, 0);
        int conteos[4] = {0};
        medir(estado, 4, shots, conteos);
        int cero = conteos[0] + conteos[2];
        const char *resultado = "Resultado no determinista";
        if (reto)
            resultado = "Sin distincion (reto)";
        else if (cero == shots)
            resultado = "Constante";
        else if (cero == 0)
            resultado = "Balanceada";
        printf("%-16s 0: %4d  1: %4d  %s\n", nombres[f], cero, shots - cero, resultado);
    }
}
