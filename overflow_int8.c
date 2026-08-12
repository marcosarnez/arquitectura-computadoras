#include <stdio.h>
#include <stdint.h>

int main(void) {
    int8_t a = 120;
    int8_t b = 20;
    int8_t suma = a + b;

    printf("a = %d\n", a);
    printf("b = %d\n", b);
    printf("a + b (int8_t) = %d\n", suma);
    printf("resultado esperado en matematica: %d\n", 120 + 20);

    return 0;
}
