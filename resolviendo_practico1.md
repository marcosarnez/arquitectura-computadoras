# Práctica: Sistemas Numéricos y Aritmética Binaria

**Materia:** Arquitectura de Computadoras  
**Estudiante:** Marcos Arnez  

---

## Nivel 1 — Conversión de bases

### 1.1 Decimal → binario

Voy dividiendo entre 2 y me quedo con los restos de abajo hacia arriba.

**45**
```
45 ÷ 2 = 22  resto 1
22 ÷ 2 = 11  resto 0
11 ÷ 2 = 5   resto 1
5  ÷ 2 = 2   resto 1
2  ÷ 2 = 1   resto 0
1  ÷ 2 = 0   resto 1
```
→ **101101₂**

**128**  
Es potencia de 2 (2⁷), así que queda un 1 y siete ceros: **10000000₂**

**255**  
255 = 256 − 1 = 2⁸ − 1, todos unos en 8 bits: **11111111₂**

**1000**
```
1000 ÷ 2 = 500  r 0
500  ÷ 2 = 250  r 0
250  ÷ 2 = 125  r 0
125  ÷ 2 = 62   r 1
62   ÷ 2 = 31   r 0
31   ÷ 2 = 15   r 1
15   ÷ 2 = 7    r 1
7    ÷ 2 = 3    r 1
3    ÷ 2 = 1    r 1
1    ÷ 2 = 0    r 1
```
→ **1111101000₂**

### 1.2 Binario → decimal

**1011₂** = 1·8 + 0·4 + 1·2 + 1·1 = **11₁₀**

**10110110₂**  
= 128 + 0 + 32 + 16 + 0 + 4 + 2 + 0 = **182₁₀**

**11111111₂** = 255₁₀ (ocho unos)

**100000000₂** = 2⁸ = **256₁₀**

### 1.3 Binario → hexadecimal

Agrupo de a 4 bits (de derecha a izquierda) y reemplazo cada grupo.

**10101100** → `1010 1100` → A C → **AC₁₆**

**111100001111** → `1111 0000 1111` → F 0 F → **F0F₁₆**

**1010101010101010** → `1010 1010 1010 1010` → A A A A → **AAAA₁₆**

### 1.4 Octal → decimal

Fórmula: d₂·8² + d₁·8¹ + d₀·8⁰

**17₈** = 1·8 + 7 = **15₁₀**

**254₈** = 2·64 + 5·8 + 4 = 128 + 40 + 4 = **172₁₀**

**777₈** = 7·64 + 7·8 + 7 = 448 + 56 + 7 = **511₁₀**

---

## Nivel 2 — Aritmética binaria

### 2.1 Sumas (con acarreo)

**1011 + 0110**
```
acarreo:  1 1 1 0
          1 0 1 1
        + 0 1 1 0
        ---------
        1 0 0 0 1
```
Chequeo: 11 + 6 = 17 → **10001₂** ✓

**11101 + 01011**
```
acarreo:  1 1 1 1 0
          1 1 1 0 1
        + 0 1 0 1 1
        -----------
        1 0 1 0 0 0
```
29 + 11 = 40 → **101000₂** ✓

### 2.2 Restas con préstamo (borrow)

**1100 − 0101** (12 − 5)

De derecha a izquierda:
- bit 0: 0 − 1 → pido prestado. Como el de al lado también es 0, se propaga hasta el bit 2.
- Resultado: **0111₂** (= 7)

**10000 − 00111** (16 − 7)

Igual, hay varios préstamos porque arranco con un 1 y varios ceros.
Resultado: **01001₂** (= 9)

### 2.3 Multiplicación: 1011 × 0101

```
      1 0 1 1      (11)
    × 0 1 0 1      (5)
    ---------
      1 0 1 1      ← ×1
    0 0 0 0        ← ×0
  1 0 1 1          ← ×1
0 0 0 0            ← ×0
-------------
  1 1 0 1 1 1
```
11 × 5 = 55 → **110111₂** ✓

---

## Nivel 3 — Complemento a dos

### 3.1 Representación en 8 bits

Método para negativos: paso a binario el valor absoluto, invierto bits y sumo 1.

| Decimal | Proceso | Complemento a 2 (8 bits) |
|---------|---------|--------------------------|
| −45 | 45 = 00101101 → invierto 11010010 → +1 | **11010011** |
| −1 | todos unos | **11111111** |
| −128 | caso especial (mínimo de 8 bits) | **10000000** |
| 27 | positivo, relleno con ceros | **00011011** |

### 3.2 Valor decimal de `11010110`

El bit más alto es 1 → es negativo.

Invierto: `00101001`  
Sumo 1: `00101010` = 42  

Entonces el número es **−42**.

(Otra forma: −128 + 64 + 16 + 4 + 2 = −128 + 86 = −42)

### 3.3 Resta 45 − 60 con C2 de 8 bits

45 = `00101101`  
60 = `00111100` → complemento a 2 de −60:

```
00111100
↓ invierto
11000011
+      1
--------
11000100   ← esto es −60
```

Ahora sumo:
```
  00101101   (45)
+ 11000100   (−60)
----------
  11110001
```

`11110001` empieza en 1 → negativo.  
Invierto: `00001110` + 1 = `00001111` = 15 → resultado **−15**.

Verificación: 45 − 60 = −15. Cuadra.

---

## Nivel 4 — Overflow y banderas

### 4.1 ¿Hay overflow?

**01111111 + 00000001**
```
  01111111   (127)
+ 00000001   (1)
----------
  10000000   (−128 en C2)
```
Suma de dos positivos y el resultado salió negativo → **sí hay overflow**.

**10000000 + 11111111**
```
  10000000   (−128)
+ 11111111   (−1)
----------
  01111111   (127)   ← también hay carry out
```
Dos negativos dan un positivo → **también hay overflow** (−128 + −1 debería ser −129, que no entra en 8 bits).

### 4.2 Carry vs overflow (con 4 bits)

A mí me quedó así la diferencia:

- **Carry (acarreo):** se prende cuando “se sale” un bit por la izquierda. Sirve más para aritmética **sin signo** (si hubo wrap-around de 0..15 en 4 bits).
- **Overflow:** se prende cuando el resultado con **signo** no tiene sentido: sumaste dos positivos y te dio negativo, o dos negativos y te dio positivo.

Ejemplo de **carry sin overflow** (4 bits):
```
  1111   (−1 con signo / 15 sin signo)
+ 0001
------
 0000   carry = 1, overflow = 0
```
Con signo: −1 + 1 = 0, bien. Sin signo: 15 + 1 = 16, se pasó → carry.

Ejemplo de **overflow sin carry**:
```
  0111   (7)
+ 0001   (1)
------
 1000   (−8)   carry = 0, overflow = 1
```
7 + 1 = 8, pero en 4 bits con signo el máximo es 7, entonces “revienta” a −8.

### 4.3 Programa en C (int8_t)

Archivo: `overflow_int8.c`

```c
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
```

Compilación y ejecución (Ubuntu):
```bash
gcc overflow_int8.c -o overflow_int8
./overflow_int8
```

Salida:
```
a = 120
b = 20
a + b (int8_t) = -116
resultado esperado en matematica: 140
```

¿Por qué sale −116?  
`int8_t` solo llega hasta 127. 120 + 20 = 140, que en binario es más de 8 bits. Al guardar solo los 8 bits menos significativos de 140:

140 − 256 = **−116**

O sea, el hardware/lenguaje hace wrap-around en módulo 256 y lo interpreta otra vez como complemento a dos. Matemáticamente está mal, pero a nivel de bits es “consistente” con el tamaño del tipo.

---

## Nivel 5 — Punto flotante (IEEE 754)

### 5.1 Representar 10.25 en simple precisión (32 bits)

Primero a binario:
- 10 = `1010`
- 0.25 = 1/4 = `0.01`

→ 10.25 = `1010.01₂`

Normalizo: `1.01001 × 2³`

- **Signo:** 0 (positivo)
- **Exponente:** 3 + 127 (bias) = 130 → `10000010`
- **Mantisa:** lo que queda después del 1 → `01001` y relleno con ceros → `01001000000000000000000`

Queda:
```
0 | 10000010 | 01001000000000000000000
```

### 5.2 Interpretar `1 10000010 01100000000000000000000`

- Signo = 1 → negativo
- Exponente = 130 − 127 = 3
- Mantisa: `1.011` = 1 + 0.25 + 0.125 = 1.375

Valor = −1.375 × 2³ = −1.375 × 8 = **−11**

### 5.3 Pérdida de precisión

En punto flotante no todos los decimales se pueden guardar exactos en binario. Números como 0.1 o 0.2 se aproximan con una fracción binaria infinita (tipo 1/10), entonces al guardarlos se corta un poquito.

Ejemplo clásico en Python:
```python
>>> 0.1 + 0.2
0.30000000000000004
>>> 0.1 + 0.2 == 0.3
False
```

No es que 0.1 + 0.2 “esté mal” en la matemática, sino que la máquina suma dos aproximaciones y el error se nota. Por eso en código a veces se compara con una tolerancia (`abs(a-b) < 1e-9`) en vez de usar `==` directo.

---

## Nivel 6 — Script de verificación

### 6.1 Script Python

Archivo: `convertidor.py`

```python
#!/usr/bin/env python3
import sys

def bits_c2(n, bits):
    minimo = -(1 << (bits - 1))
    maximo = (1 << (bits - 1)) - 1
    if n < minimo or n > maximo:
        return None
    if n >= 0:
        return format(n, f"0{bits}b")
    return format((1 << bits) + n, f"0{bits}b")

def main():
    if len(sys.argv) != 2:
        print("Uso: python3 convertidor.py <entero>")
        sys.exit(1)

    n = int(sys.argv[1])
    print(f"Numero: {n}")
    print(f"Binario:      {bin(n)}")
    print(f"Octal:        {oct(n)}")
    print(f"Hexadecimal:  {hex(n)}")

    for bits in (8, 16, 32):
        rep = bits_c2(n, bits)
        if rep is None:
            print(f"C2 {bits} bits:  fuera de rango (no aplica)")
        else:
            print(f"C2 {bits} bits:  {rep}")

    if -128 <= n <= 127:
        print("Overflow en 8 bits con signo: NO")
    else:
        print("Overflow en 8 bits con signo: SI")

if __name__ == "__main__":
    main()
```

Pruebas (casos límite y cero):

```bash
python3 convertidor.py 127
python3 convertidor.py 128
python3 convertidor.py -128
python3 convertidor.py -129
python3 convertidor.py 0
```

**Salidas:**

```
Numero: 127
Binario:      0b1111111
Octal:        0o177
Hexadecimal:  0x7f
C2 8 bits:  01111111
C2 16 bits:  0000000001111111
C2 32 bits:  00000000000000000000000001111111
Overflow en 8 bits con signo: NO
```

```
Numero: 128
Binario:      0b10000000
Octal:        0o200
Hexadecimal:  0x80
C2 8 bits:  fuera de rango (no aplica)
C2 16 bits:  0000000010000000
C2 32 bits:  00000000000000000000000010000000
Overflow en 8 bits con signo: SI
```

```
Numero: -128
Binario:      -0b10000000
Octal:        -0o200
Hexadecimal:  -0x80
C2 8 bits:  10000000
C2 16 bits:  1111111110000000
C2 32 bits:  11111111111111111111111110000000
Overflow en 8 bits con signo: NO
```

```
Numero: -129
Binario:      -0b10000001
Octal:        -0o201
Hexadecimal:  -0x81
C2 8 bits:  fuera de rango (no aplica)
C2 16 bits:  1111111101111111
C2 32 bits:  11111111111111111111111101111111
Overflow en 8 bits con signo: SI
```

```
Numero: 0
Binario:      0b0
Octal:        0o0
Hexadecimal:  0x0
C2 8 bits:  00000000
C2 16 bits:  0000000000000000
C2 32 bits:  00000000000000000000000000000000
Overflow en 8 bits con signo: NO
```

Se nota claro el límite: 127 y −128 entran justo, pero 128 y −129 ya no caben en 8 bits con signo.

### 6.2 Overflow silencioso en un sensor

Si un sensor manda la temperatura en un `int8_t` (rango −128 a 127) y de golpe la lectura real pasa de 127 °C, el valor no “avisa” que se pasó: hace wrap y puede aparecer algo como −128 °C (o un negativo raro). El sistema embebido seguiría trabajando como si esa temperatura fuera válida. En la práctica eso es peligroso porque un controlador podría encender calefacción creyendo que está helado, o apagar un ventilador/alarma cuando en realidad el equipo se está sobrecalentando. El overflow silencioso engaña a la lógica de control sin tirar error, y ahí está el riesgo.

---

Listo. Los archivos del práctico están en el repo: `overflow_int8.c` y `convertidor.py`.
