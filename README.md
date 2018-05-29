# Associativity of Floating Point Arithmetic
This repository represents an effort to address [usnistgov/discuss #8][_git]
by exploring floating-point rounding errors using float and MPFR data types

## Executive Summary
If you value associative math over standards compliance, simply choose optimization
level `-Ofast`. This level starts with `-O3` and adds corner-cutting flags, the
most relevant of which is `-funsafe-math-optimizations`, which allows the compiler
to *change your numbers* to behave in ways it thinks you want, rather than how
IEEE 754 expects.

I am not advocating `-funsafe-math-optimizations` without serious debate.
It bears repeating that this flag *will change your numbers* in ways that
the internationally agreed standards forbid. *You have been warned.*

## Dependencies
- A C compiler, *e.g.* [GNU Compiler Collection][_gcc]
- Standard C libraries
- GNU [Make][_mak]
- [MPFR][_gnu] library headers and runtime, version 4 or greater
  - On Debian derivatives, `$ apt-get install libmpfr4 libmpfr-dev`

### Test 1: Basic Associative Addition
When addition is associative, we expect the sum of three terms, `a+b+c`, to
be independent of computation as `(a + b) + c` or `a + (b + c)`. Due to the
binary representation of floating-point numbers ([IEEE 754][_eee]), *or* due to
out-of-order execution on some CPUs, this will not be the case for numerical
approximations to floating-point summation.

#### Usage and Results
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

#### Built-in floating point representation
```bash
  $ make std
  gcc -O3 -Wall associative.c -o std -lm && ./std
  | a           bin(a)                       | bin(a+b+c)                (a+b)+c     a+(b+c)     | equal |
  | 1.000000000 1.0                          | 1.0                       1.000000000 1.000000000 | 1     |
  | 0.500000000 0.1                          | 0.1                       0.500000000 0.500000000 | 1     |
  | 0.333333343 0.0101010101010101010101011  | 0.01010101010101010101011 0.333333373 0.333333343 | 0     |
  | 0.250000000 0.01                         | 0.01                      0.250000000 0.250000000 | 1     |
  | 0.200000003 0.00110011001100110011001101 | 0.0011001100110011001101  0.200000048 0.200000003 | 0     |
  | 0.166666672 0.00101010101010101010101011 | 0.00101010101010101010101 0.166666627 0.166666672 | 0     |
  | 0.142857149 0.00100100100100100100100101 | 0.00100100100100100100101 0.142857194 0.142857149 | 0     |
  | 0.125000000 0.001                        | 0.001                     0.125000000 0.125000000 | 1     |
```

#### GNU MPFR floating-point representation
```bash
  $ make gmp
  gcc -O3 -Wall -include "mpfr.h" associative.c -o gmp -lm -lmpfr && ./gmp
  | a           bin(a)               | bin(a+b+c)        (a+b)+c     a+(b+c)     | equal |
  | 1.000000000 1.0                  | 1.0               1.000000000 1.000000000 | 1     |
  | 0.500000000 0.1                  | 0.1               0.500000000 0.500000000 | 1     |
  | 0.333328247 0.0101010101010101   | 0.01010101010101  0.333312988 0.333328247 | 0     |
  | 0.250000000 0.01                 | 0.01              0.250000000 0.250000000 | 1     |
  | 0.199996948 0.0011001100110011   | 0.001100110011001 0.199981689 0.199996948 | 0     |
  | 0.166664124 0.00101010101010101  | 0.001010101010101 0.166656494 0.166664124 | 0     |
  | 0.142856598 0.001001001001001001 | 0.001001001001001 0.142852783 0.142856598 | 0     |
  | 0.125000000 0.001                | 0.001             0.125000000 0.125000000 | 1     |
```

#### Optimized floating-point representation
`-funsafe-math-optimizations` enables optimizations that allow arbitrary reassociations and
transformations with no accuracy guarantees. It also does not try to preserve the sign of zeros.
```bash
  $ make unsafe
  gcc -O3 -Wall -funsafe-math-optimizations associative.c -o unsafe -lm && ./unsafe
  | a           bin(a)                       | bin(a+b+c)                   (a+b)+c     a+(b+c)     | equal |
  | 1.000000000 1.0                          | 1.0                          1.000000000 1.000000000 | 1     |
  | 0.500000000 0.1                          | 0.1                          0.500000000 0.500000000 | 1     |
  | 0.333333343 0.0101010101010101010101011  | 0.0101010101010101010101011  0.333333343 0.333333343 | 1     |
  | 0.250000000 0.01                         | 0.01                         0.250000000 0.250000000 | 1     |
  | 0.200000003 0.00110011001100110011001101 | 0.00110011001100110011001101 0.200000003 0.200000003 | 1     |
  | 0.166666672 0.00101010101010101010101011 | 0.00101010101010101010101011 0.166666672 0.166666672 | 1     |
  | 0.142857149 0.00100100100100100100100101 | 0.00100100100100100100100101 0.142857149 0.142857149 | 1     |
  | 0.125000000 0.001                        | 0.001                        0.125000000 0.125000000 | 1     |
```

#### Discussion
As [@JRMatey-NIST noted][_jmt], this is a straight-forward byproduct of the binary
representation of floating-point numbers. Exact representation is possible for integral
exponents of 2; any other number incurs [rounding error][_rnd]. This is good to know,
especially for codes that frequently increment large values by small amounts.

Since many scientific computing applications model diffusive processes (heat transfer,
mass diffusion, etc.), the effect is expected to be small: the perturbations caused
by round-off error will be smoothed out by the stable numerical scheme without any
additional effort on the part of the programmer.

### Test 2: Shuffled Sum
Ideally, the sum `10*0.001 + 9*0.01 + 9*0.1 + 9*1. + 9*10 + 9*100 + 9*1000 = 10000`.
However, due to the same floating point representation problem, variations arise
from the order of summation. As a demonstration, this program will generate a vector
of 64 numbers (10 + 9*6), then for each of 1 million trials, the same vector
gets shuffled before summing. The histogram of values is then reported.
For additional details, see the [original thread][_git].

#### Usage and Results
There is only one variant, `shuffle`, which can be built using the Makefile or the
command listed below.

#### Built-in floating point representation
```bash
  $ make shuffle 
  g++ -O -Wall -pedantic -std=c++11 -frounding-math shuffle-sum.cpp -o shuffle && ./shuffle
   9999.99414062500000000000000000:  0.033300001 %
   9999.99511718750000000000000000:  0.624599993 %
   9999.99609375000000000000000000:  4.240699768 %
   9999.99707031250000000000000000: 15.360699654 %
   9999.99804687500000000000000000: 34.903400421 %
   9999.99902343750000000000000000: 33.413898468 %
  10000.00000000000000000000000000: 11.187700272 %
  10000.00097656250000000000000000:  0.235699996 %
```

<!--References-->
[_eee]: https://en.wikipedia.org/wiki/IEEE_754
[_gcc]: https://gcc.gnu.org/
[_git]: https://github.com/usnistgov/discuss/issues/8
[_gnu]: http://www.mpfr.org/
[_jmt]: https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151
[_mak]: https://www.gnu.org/software/make/
[_rnd]: https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#680
