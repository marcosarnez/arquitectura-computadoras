#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323846

/* Los circuitos de esta guía usan amplitudes reales. El bit q0 es el menos significativo. */
static inline void puerta(double *estado, unsigned n, unsigned q,
                          unsigned control, double a, double b, double c, double d)
{
    unsigned bit = 1u << q;
    for (unsigned i = 0; i < (1u << n); ++i) {
        if ((i & bit) || (i & control) != control)
            continue;
        unsigned j = i | bit;
        double cero = estado[i], uno = estado[j];
        estado[i] = a * cero + b * uno;
        estado[j] = c * cero + d * uno;
    }
}

static inline void h(double *estado, unsigned n, unsigned q)
{
    double s = sqrt(0.5);
    puerta(estado, n, q, 0, s, s, s, -s);
}

static inline void x(double *estado, unsigned n, unsigned q, unsigned control)
{
    puerta(estado, n, q, control, 0, 1, 1, 0);
}

static inline double uniforme(void)
{
    return (rand() + 0.5) / (RAND_MAX + 1.0);
}

static inline void medir(const double *estado, unsigned dimension, int shots, int *conteos)
{
    for (int shot = 0; shot < shots; ++shot) {
        double muestra = uniforme();
        unsigned i = 0;
        for (; i + 1 < dimension; ++i) {
            muestra -= estado[i] * estado[i];
            if (muestra < 0)
                break;
        }
        ++conteos[i];
    }
}

#endif
