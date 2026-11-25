#include <stdio.h>
#include "simulador.h"

int main(void)
{
    const int shots = 10000;
    const double angulos[] = {0, PI / 4, PI / 2, PI};
    const char *nombres[] = {"0", "pi/4", "pi/2", "pi"};

    srand(42);
    printf("LAB 2 | Rotaciones Ry (%d mediciones por angulo)\n", shots);
    printf("%-8s %10s %11s %10s %11s\n", "Angulo", "P(0) sim.", "P(0) teoria", "P(1) sim.", "P(1) teoria");
    for (unsigned i = 0; i < sizeof angulos / sizeof angulos[0]; ++i) {
        double c = cos(angulos[i] / 2), s = sin(angulos[i] / 2);
        double estado[2] = {1, 0};
        int conteos[2] = {0};
        puerta(estado, 1, 0, 0, c, -s, s, c);
        medir(estado, 2, shots, conteos);
        printf("%-8s %9.2f%% %10.2f%% %9.2f%% %10.2f%%\n", nombres[i],
               100.0 * conteos[0] / shots, 100.0 * c * c,
               100.0 * conteos[1] / shots, 100.0 * s * s);
    }
}
