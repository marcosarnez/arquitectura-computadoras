#include <stdio.h>
#include "simulador.h"

int main(void)
{
    const int shots = 10000;
    const double a = cos(PI / 6), b = sin(PI / 6);
    double estado[8] = {a, b};
    h(estado, 3, 1);
    x(estado, 3, 2, 1u << 1);
    x(estado, 3, 1, 1u << 0);
    h(estado, 3, 0);

    int conteos[2] = {0};
    double fidelidad = 0, acumulada[4] = {0};
    double ramas[4][2];
    for (unsigned bits = 0; bits < 4; ++bits) {
        /* Al medir Alice, se conserva una rama del vector. Su norma al cuadrado
           es la probabilidad de obtener esos dos bits clásicos. */
        double *bob = ramas[bits];
        bob[0] = estado[bits];
        bob[1] = estado[bits | 4u];
        double prob = bob[0] * bob[0] + bob[1] * bob[1];
        acumulada[bits] = prob + (bits ? acumulada[bits - 1] : 0);

        if (bits & 2u)
            x(bob, 1, 0, 0);
        if (bits & 1u)
            bob[1] = -bob[1];

        /* La suma de los solapamientos al cuadrado ya pondera cada rama. */
        double solapamiento = a * bob[0] + b * bob[1];
        fidelidad += solapamiento * solapamiento;
        bob[0] /= sqrt(prob);
        bob[1] /= sqrt(prob);
    }

    srand(42);
    for (int shot = 0; shot < shots; ++shot) {
        double muestra = uniforme();
        unsigned bits = 0;
        while (bits < 3 && muestra >= acumulada[bits])
            ++bits;
        medir(ramas[bits], 2, 1, conteos);
    }
    printf("LAB 4 | Teletransportacion (%d mediciones)\n", shots);
    printf("Entrada: Ry(pi/3)|0> | Salida: qubit q2 de Bob\n");
    printf("|0>: %5d (%6.2f%%) | Teoria: %.2f%%\n", conteos[0], 100.0 * conteos[0] / shots, 100.0 * a * a);
    printf("|1>: %5d (%6.2f%%) | Teoria: %.2f%%\n", conteos[1], 100.0 * conteos[1] / shots, 100.0 * b * b);
    printf("Fidelidad del estado de Bob: %.6f\n", fidelidad);
}
