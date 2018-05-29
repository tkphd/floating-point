# Associativity of Floating Point Arithmetic
This repository represents an effort to address [usnistgov/discuss #8][_git]
by exploring floating-point rounding errors using float and MPFR data types

## Problem Statement
When addition is associative, we expect the sum of three terms, `a+b+c`, to
be independent of computation as `(a + b) + c` or `a + (b + c)`. Due to the
binary representation of floating-point numbers ([IEEE 754][_eee]), *or* due to
out-of-order execution on some CPUs, this will not be the case for numerical
approximations to floating-point summation.

## Executive Summary
This is why people pass `-ffast-math` to gcc (or equivalent flag to other C compilers).

## Usage and Results
The code has three variants: `std` (standard representation), `gmp` (GNU MP
representation), and `unsafe` (`std` with unsafe optimizations). Use the
included Makefile or the commands in the following sections to compile the
executables.

Each program will output the value of `a` in decimal and binary form, `d` in
binary form, and both of the associative expressions in decimal form. When
the expressions agree, a `1` is printed in the last column; otherwise, it
will be `0`. The binary form of a floating-point number is similar to an
integer binary, with each bit representing a power of two that decreases
from `0` at the decimal. Therefore,
```
1   = 2^( 0)  = 1.00
1/2 = 2^(-1)  = 0.10
1/4 = 2^(-2)  = 0.01
3/4 = 1/2+1/4 = 0.11
```

### Dependencies
- A C compiler, *e.g.* [GNU Compiler Collection][_gcc]
- Standard C libraries
- GNU [Make][_mak]
- [MPFR][_gnu] library headers and runtime, version 4 or greater
  - On Debian derivatives, `$ apt-get install libmpfr4 libmpfr-dev`

### Built-in floating point representation
```bash
  $ make std
  gcc -O3 -Wall associative.c -o std -lm && ./std
  |  a            bin(a)                          | bin(d)                           l             r           | equal |
  |  1.000000000  1.0                             | 1.0                              1.000000000   1.000000000 | 1     |
  |  0.500000000  0.1                             | 0.1                              0.500000000   0.500000000 | 1     |
  |  0.333333343  0.0101010101010101010101011     | 0.01010101010101010101011        0.333333373   0.333333343 | 0     |
  |  0.250000000  0.01                            | 0.01                             0.250000000   0.250000000 | 1     |
  |  0.200000003  0.00110011001100110011001101    | 0.0011001100110011001101         0.200000048   0.200000003 | 0     |
  |  0.166666672  0.00101010101010101010101011    | 0.00101010101010101010101        0.166666627   0.166666672 | 0     |
  |  0.142857149  0.00100100100100100100100101    | 0.00100100100100100100101        0.142857194   0.142857149 | 0     |
  |  0.125000000  0.001                           | 0.001                            0.125000000   0.125000000 | 1     |
  |  0.111111112  0.000111000111000111000111001   | 0.000111000111000111001          0.111111164   0.111111112 | 0     |
  |  0.100000001  0.000110011001100110011001101   | 0.00011001100110011001101        0.100000024   0.100000001 | 0     |
  |  0.090909094  0.0001011101000101110100011     | 0.00010111010001011101001        0.090909123   0.090909094 | 0     |
  |  0.083333336  0.000101010101010101010101011   | 0.00010101010101010101011        0.083333373   0.083333336 | 0     |
  |  0.076923080  0.000100111011000100111011001   | 0.0001001110110001001111         0.076923132   0.076923080 | 0     |
  |  0.071428575  0.000100100100100100100100101   | 0.0001001001001001001001         0.071428537   0.071428575 | 0     |
  |  0.066666670  0.000100010001000100010001001   | 0.00010001000100010001001        0.066666722   0.066666670 | 0     |
  |  0.062500000  0.0001                          | 0.0001                           0.062500000   0.062500000 | 1     |
```

### GNU MPFR floating-point representation
```bash
  $ make gmp
  gcc -O3 -Wall -include "mpfr.h" associative.c -o gmp -lm -lmpfr && ./gmp
  |  a            bin(a)                          | bin(d)                           l             r           | equal |
  |  1.000000000  1.0                             | 1.0                              1.000000000   1.000000000 | 1     |
  |  0.500000000  0.1                             | 0.1                              0.500000000   0.500000000 | 1     |
  |  0.333328247  0.0101010101010101              | 0.01010101010101                 0.333312988   0.333328247 | 0     |
  |  0.250000000  0.01                            | 0.01                             0.250000000   0.250000000 | 1     |
  |  0.199996948  0.0011001100110011              | 0.001100110011001                0.199981689   0.199996948 | 0     |
  |  0.166664124  0.00101010101010101             | 0.001010101010101                0.166656494   0.166664124 | 0     |
  |  0.142856598  0.001001001001001001            | 0.001001001001001                0.142852783   0.142856598 | 0     |
  |  0.125000000  0.001                           | 0.001                            0.125000000   0.125000000 | 1     |
  |  0.111110687  0.000111000111000111            | 0.000111000111                   0.111083984   0.111110687 | 0     |
  |  0.099998474  0.00011001100110011             | 0.0001100110011                  0.099975586   0.099998474 | 0     |
  |  0.090908051  0.000101110100010111            | 0.00010111010001                 0.090881348   0.090908051 | 0     |
  |  0.083332062  0.000101010101010101            | 0.00010101010101                 0.083312988   0.083332062 | 0     |
  |  0.076921463  0.0001001110110001001           | 0.000100111011                   0.076904297   0.076921463 | 0     |
  |  0.071428299  0.0001001001001001001           | 0.0001001001001                  0.071411133   0.071428299 | 0     |
  |  0.066665649  0.0001000100010001              | 0.000100010001                   0.066650391   0.066665649 | 0     |
  |  0.062500000  0.0001                          | 0.0001                           0.062500000   0.062500000 | 1     |
```

### Optimized floating-point representation
`-funsafe-math-optimizations` enables optimizations that allow arbitrary reassociations and
transformations with no accuracy guarantees. It also does not try to preserve the sign of zeros.
```bash
  $ make unsafe 
  gcc -O3 -Wall -funsafe-math-optimizations associative.c -o unsafe -lm && ./unsafe
  |  a            bin(a)                          | bin(d)                           l             r           | equal |
  |  1.000000000  1.0                             | 1.0                              1.000000000   1.000000000 | 1     |
  |  0.500000000  0.1                             | 0.1                              0.500000000   0.500000000 | 1     |
  |  0.333333343  0.0101010101010101010101011     | 0.0101010101010101010101011      0.333333343   0.333333343 | 1     |
  |  0.250000000  0.01                            | 0.01                             0.250000000   0.250000000 | 1     |
  |  0.200000003  0.00110011001100110011001101    | 0.00110011001100110011001101     0.200000003   0.200000003 | 1     |
  |  0.166666672  0.00101010101010101010101011    | 0.00101010101010101010101011     0.166666672   0.166666672 | 1     |
  |  0.142857149  0.00100100100100100100100101    | 0.00100100100100100100100101     0.142857149   0.142857149 | 1     |
  |  0.125000000  0.001                           | 0.001                            0.125000000   0.125000000 | 1     |
  |  0.111111112  0.000111000111000111000111001   | 0.000111000111000111000111001    0.111111112   0.111111112 | 1     |
  |  0.100000001  0.000110011001100110011001101   | 0.000110011001100110011001101    0.100000001   0.100000001 | 1     |
  |  0.090909094  0.0001011101000101110100011     | 0.0001011101000101110100011      0.090909094   0.090909094 | 1     |
  |  0.083333336  0.000101010101010101010101011   | 0.000101010101010101010101011    0.083333336   0.083333336 | 1     |
  |  0.076923080  0.000100111011000100111011001   | 0.000100111011000100111011001    0.076923080   0.076923080 | 1     |
  |  0.071428575  0.000100100100100100100100101   | 0.000100100100100100100100101    0.071428575   0.071428575 | 1     |
  |  0.066666670  0.000100010001000100010001001   | 0.000100010001000100010001001    0.066666670   0.066666670 | 1     |
  |  0.062500000  0.0001                          | 0.0001                           0.062500000   0.062500000 | 1     |
```

```bash
  $ gcc -O -Wall -pedantic -include "mpfr.h" -funsafe-math-optimizations associative.c -o associative -lmpfr && ./associative
  |  a             b             c           |  d             l             r           | equal |
  |  0.500000000   1.000000000  -1.000000000 |  0.500000000   0.500000000   0.500000000 | 1     |
  |  0.250000000   1.000000000  -1.000000000 |  0.250000000   0.250000000   0.250000000 | 1     |
  |  0.166664124   1.000000000  -1.000000000 |  0.166656494   0.166656494   0.166664124 | 0     |
  |  0.125000000   1.000000000  -1.000000000 |  0.125000000   0.125000000   0.125000000 | 1     |
  |  0.099998474   1.000000000  -1.000000000 |  0.099975586   0.099975586   0.099998474 | 0     |
  |  0.083332062   1.000000000  -1.000000000 |  0.083312988   0.083312988   0.083332062 | 0     |
  |  0.071428299   1.000000000  -1.000000000 |  0.071411133   0.071411133   0.071428299 | 0     |
  |  0.062500000   1.000000000  -1.000000000 |  0.062500000   0.062500000   0.062500000 | 1     |
  |  0.055555344   1.000000000  -1.000000000 |  0.055541992   0.055541992   0.055555344 | 0     |
  |  0.049999237   1.000000000  -1.000000000 |  0.049987793   0.049987793   0.049999237 | 0     |
  |  0.045454025   1.000000000  -1.000000000 |  0.045440674   0.045440674   0.045454025 | 0     |
  |  0.041666031   1.000000000  -1.000000000 |  0.041656494   0.041656494   0.041666031 | 0     |
  |  0.038460732   1.000000000  -1.000000000 |  0.038452148   0.038452148   0.038460732 | 0     |
  |  0.035714149   1.000000000  -1.000000000 |  0.035705566   0.035705566   0.035714149 | 0     |
  |  0.033332825   1.000000000  -1.000000000 |  0.033325195   0.033325195   0.033332825 | 0     |
  |  0.031250000   1.000000000  -1.000000000 |  0.031250000   0.031250000   0.031250000 | 1     |
```

### Discussion
As [@JRMatey-NIST noted][_jmt], this is a straight-forward byproduct of the binary
representation of floating-point numbers. Exact representation is possible for integral
exponents of 2; any other number incurs [rounding error][_rnd]. This is good to know,
especially for codes that frequently increment large values by small amounts.

Since many scientific computing applications model diffusive processes (heat transfer,
mass diffusion, etc.), the effect is expected to be small: the perturbations caused
by round-off error will be smoothed out by the stable numerical scheme without any
additional effort on the part of the programmer.

<!--References-->
[_eee]: https://en.wikipedia.org/wiki/IEEE_754
[_gcc]: https://gcc.gnu.org/
[_git]: https://github.com/usnistgov/discuss/issues/8
[_gnu]: http://www.mpfr.org/
[_jmt]: https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151
[_mak]: https://www.gnu.org/software/make/
[_rnd]: https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#680
