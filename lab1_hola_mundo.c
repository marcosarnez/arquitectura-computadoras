#include <stdio.h>
#include <string.h>
#include "simulador.h"

int main(int argc, char **argv)
{
    int reto = argc == 2 && strcmp(argv[1], "--psi-plus") == 0;
    if (argc > 1 && !reto) {
        fprintf(stderr, "Uso: %s [--psi-plus]\n", argv[0]);
        return 1;
    }

    const int shots = 10000;
    double estado[4] = {1, 0, 0, 0};
    int conteos[4] = {0};
    h(estado, 2, 0);
    x(estado, 2, 1, 1u << 0);
    if (reto)
        x(estado, 2, 1, 0);

    srand(42);
    medir(estado, 4, shots, conteos);
    printf("LAB 1 | Estado de Bell %s (%d mediciones)\n", reto ? "Psi+" : "Phi+", shots);
    printf("Circuito: H(q0) -> CNOT(q0, q1)%s -> medir\n", reto ? " -> X(q1)" : "");
    for (unsigned i = 0; i < 4; ++i)
        printf("|%u%u>: %5d (%6.2f%%)\n", i >> 1, i & 1u, conteos[i], 100.0 * conteos[i] / shots);
}
