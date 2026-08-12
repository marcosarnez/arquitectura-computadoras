#!/usr/bin/env python3
"""Convierte un entero decimal a bin/oct/hex y complemento a dos."""

import sys


def bits_c2(n: int, bits: int) -> str | None:
    minimo = -(1 << (bits - 1))
    maximo = (1 << (bits - 1)) - 1
    if n < minimo or n > maximo:
        return None
    if n >= 0:
        return format(n, f"0{bits}b")
    return format((1 << bits) + n, f"0{bits}b")


def main() -> None:
    if len(sys.argv) != 2:
        print("Uso: python3 convertidor.py <entero>")
        sys.exit(1)

    try:
        n = int(sys.argv[1])
    except ValueError:
        print("Error: tiene que ser un entero")
        sys.exit(1)

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
