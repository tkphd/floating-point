# A Primer on Floating-Point Arithmetic and IEEE 754

This repository addresses [usnistgov/discuss#8][git] by exploring
floating-point rounding errors through the lens of the associative
property of addition. The programs herein demonstrate that deviations
from mathematical precision are due to the standard representation of
floating-point numbers in a binary data type, in compliance with
[IEEE 754][eee]. The evidence presented here does not support any
role of the CPU, instruction sets, or [dynamic execution][dyn] in the
deviations.

*tl;dr:* If strict adherence to mathematical law is required, use a
[high-precision math library][lib]. For example, instead of the
built-in data types, use

- [MPFR][mpf] for C/C++.
- [MPMath][mpm] for Python.

[![DOI](https://zenodo.org/badge/135336811.svg)][zen]

This primer was written by Trevor Keller <trevor.keller@nist.gov>.
It is intended for educational purposes only.
Please cite this work:

> Trevor Keller, "A primer on floating-point arithmetic and IEEE 754."
> National Institute of Standards and Technology (2018):
> DOI [10.5281/zenodo.6524704][doi].

## Table of Contents

1. [Three Term Addition](#three-term-addition)
2. [Shuffled Summation](#shuffled-summation)
3. [Out-of-Order Execution](#out-of-order)
4. [Conclusions](#conclusions)
5. [Further Reading](#further-reading)

## Dependencies

To build the C and C++ programs in this repository, you will need:

- C and C++ compilers with standard libraries, *e.g.* [gcc][gcc]
- [Make][mak] build tool
- [MPFR][mpf] library and headers, version 4 or greater
  
On Debian derivatives (Ubuntu, Mint), these are all packaged:

```bash
apt install gcc libmpfr6 libmpfr-dev make
```

## Basic Premise

Addition of real numbers is [associative][add]: inserting parentheses
must not change the computed sum. For example,

```math
(1+2)+(3+4)+5 = (1+2)+3+(4+5) = 1+(2+3)+(4+5) = 15
```

The [two's complement][two] representation of integers naturally
supports associativity, within the representable range of `2^(n-1)-1`
for an `n`-bit representation. However, fractional real numbers incur
a round-off error since infinite digits cannot practically be stored.
The chief exception to this general rule is that real fractions
formed by the sum of powers of two *are* exactly represented, as a
natural consequence of the data format. It should be noted that two's
complement is *not* the internal representation used for floating
point numbers, but can be used to visualize floating point
representations.

## <a name="three-term-addition"></a>Three Term Addition

When addition is associative, we expect the sum of three terms,
`a+b+c`, to be independent of computation as `(a+b)+c` or `a+(b+c)`.
Due to the binary representation of floating-point numbers (specified
by [IEEE 754][eee]), *or* due to out-of-order execution on some CPUs,
this will not be the case for numerical approximations to
floating-point summation.

### Results of Three-Term Addition

The code has three variants:

- `std` (standard representation)
- `mpf` (GNU MPFR representation)
- `unsafe` (`std` with unsafe optimizations).

Use the included Makefile or the commands in the following sections
to compile the executables.

Each program will output the value of `a` in decimal and binary form,
`a+b+c` in binary form, and both of the associative expressions in
decimal form. When the expressions agree, a `1` is printed in the
last column; otherwise, it will be `0`. The binary form of a
floating-point number is similar to an integer binary, with each bit
representing a power of two that decreases from `0` at the decimal.
Therefore,

```math
1 = 2^( 0) = 1.00
½ = 2^(-1) = 0.10
¼ = 2^(-2) = 0.01
¾ = ½ + ¼  = 0.11
```

#### Built-in floating point representation

```bash
$ make std
gcc -O3 -Wall addition.c -o std -lm && ./std
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

#### Multi-Precision Floating-point Representation

Using precision identical to the built-in float, we have the same
result:

```bash
$ make spf
gcc -O3 -Wall -D_SINGLE_ -pedantic -include "mpfr.h" addition.c -o spf -lm -lmpfr && ./spf
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

However, setting the precision of a long double,

```bash
$ make mpf
gcc -O3 -Wall -pedantic -include "mpfr.h" addition.c -o mpf -lm -lmpfr && ./mpf
| a           bin(a)                      | bin(a+b+c)                  (a+b)+c     a+(b+c)     | equal |
| 1.000000000 1.0                         | 1.0                         1.000000000 1.000000000 | 1     |
| 0.500000000 0.1                         | 0.1                         0.500000000 0.500000000 | 1     |
| 0.333333313 0.010101010101010101010101  | 0.010101010101010101010101  0.333333313 0.333333313 | 1     |
| 0.250000000 0.01                        | 0.01                        0.250000000 0.250000000 | 1     |
| 0.199999988 0.001100110011001100110011  | 0.001100110011001100110011  0.199999988 0.199999988 | 1     |
| 0.166666657 0.0010101010101010101010101 | 0.0010101010101010101010101 0.166666657 0.166666657 | 1     |
| 0.142857134 0.001001001001001001001001  | 0.001001001001001001001001  0.142857134 0.142857134 | 1     |
| 0.125000000 0.001                       | 0.001                       0.125000000 0.125000000 | 1     |
```

Using a library therefore allows the programmer to choose an
appropriate level of precision, as well as the rounding scheme and
deterministic orders of operations.

#### Unsafe floating-point representation

Per the [Using the GNU Compiler Collection(GCC) &sect;3.10][gcc],
`-funsafe-math-optimizations` enables optimizations that

- assume both arguments and results are valid
- violate IEEE and ANSI standards
- change the floating-point unit [control word][fpu]
- cause programs that rely on exact implementation of IEEE or ISO
  rules to fail
- may yield faster code for programs that do not require the
  guarantees of these specifications.

In other words, it is ill-advised for production code. However,

```bash
$ make unsafe
gcc -O3 -Wall -funsafe-math-optimizations addition.c -o unsafe -lm && ./unsafe
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

### Discussion of Three-Term Addition

As [@JRMatey-NIST noted][jmt], this is a straight-forward byproduct
of the binary representation of floating-point numbers. Exact
representation is possible for integral exponents of 2; any other
number incurs [rounding error][rnd]. This is good to know, especially
for codes that frequently increment large values by small amounts.

Since many scientific computing applications model diffusive
processes (heat transfer, mass diffusion, etc.), the effect is
expected to be small: the perturbations caused by round-off error
will be smoothed out by the stable numerical scheme without any
additional effort on the part of the programmer.

## <a name="shuffled-summation"></a>Shuffled Summation

Ideally, the sequence of decimals (powers of 10)

```math
  10    9     9
---- + --- + -- + 9 + 90 + 900 + 9000 = 10⁴ = 10000.
1000   100   10
```

However, due to the same floating point representation problem,
variations arise from the order of summation. As a demonstration,
this program will generate a vector of 64 numbers (`10 + 9×6`), then
for each of 1 million trials, the same vector gets shuffled before
summing. The histogram of values is then reported. For additional
details, see the [original thread][_git].

Similarly, the sequence of binaries (powers of 2)

```math
 8     8    8    9
--  + -- + -- + -- + 8 + 16 + 32 + 64 = 2⁷ = 128.
16     8    4    2
```

The program will generate a vector of 65 numbers (`9 + 8×7`) and, for
each of 1 million trials, shuffle the vector before summing. Due to
the exact representation of powers-of-two, only one result (`128`) is
expected for all million shuffles.

### Results of Shuffled Summation

There are two variants, `shuffle` and `shuffle10`, which can be built
using the Makefile or the command listed below.

#### Floating point representation of decimal sequence `∑10ⁿ`

```bash
$ make shuffle10
g++ -O3 -Wall -pedantic -std=c++11 -DDECIMAL summation.cpp -o shuffle && ./shuffle
 9999.99414062500000000000000000:  0.033300001 %
 9999.99511718750000000000000000:  0.624599993 %
 9999.99609375000000000000000000:  4.240699768 %
 9999.99707031250000000000000000: 15.360699654 %
 9999.99804687500000000000000000: 34.903400421 %
 9999.99902343750000000000000000: 33.413898468 %
10000.00000000000000000000000000: 11.187700272 %
10000.00097656250000000000000000:  0.235699996 %
```

#### Floating point representation of binary sequence `∑2ⁿ`

```math
$ make shuffle
g++ -O3 -Wall -pedantic -std=c++11 summation.cpp -o shuffle && ./shuffle
128.00000000000000000000000000: 100.000000000 %
```

### Discussion of Shuffled Summation

The sequence comprised exclusively of powers of 2 is represented
exactly, *i.e.* 1 million repetitions produce the same result,
exactly `128`, every time. The sequence of powers of 10 is
approximate, with the exact result, `10000`, computed in only 11 %
of the million repetitions.

## <a name="out-of-order"></a>Out-of-Order Execution

[Out-of-order execution][ooe] is a common method to avoid processor
pipeline stalls: instructions are fetched in the order provided by
the compiler, but executed in a way that minimizes wasted processor
cycles. While modern CPUs implement out-of-order execution with
in-order completion, a few architectures feature out-of-order
*completion* of the compiler-supplied instructions, *e.g.* Intel's
[P6 (Pentium Pro/II/III)][ip6], [Core][ico], and [Silvermont (Atom,
Knights Landing)][ism]. Since the sequence of mathematical operations
would be non-deterministic, these processors could exhibit poor
reproducibility of floating-point arithmetic.

### Results of Out-of-Order Execution

Variants of the [shuffled summation](#shuffled-summation) and
[three-term addition](#three-term-addition) codes can be used to
compare results between Intel [Xeon (Sandy Bridge)][isb] and [Knights
Landing (Silvermont)][ism] processors. Since the KNL hardware is
novel, the [Intel C++ Compiler][icc] is preferred over [GCC][gcc]; it
sets [`-fp-model fast=1`][fpm] by default, which is equivalent to
`-Ofast` in GCC and must be disabled to provide a valid comparison
using standards-compliant arithmetic. The variants, `phi` and
`shufflePhi`, can be built using the Makefile or the commands below.

```bash
$ make phi
icc -O3 -Wall -xmic-avx512 -fp-model precise addition.c -o phi && ./phi
|  a            bin(a)                       | bin(a+b+c)                      (a+b)+c       a+(b+c)     | equal |
|  1.000000000  1.0                          | 1.0                             1.000000000   1.000000000 | 1     |
|  0.500000000  0.1                          | 0.1                             0.500000000   0.500000000 | 1     |
|  0.333333343  0.0101010101010101010101011  | 0.01010101010101010101011       0.333333373   0.333333343 | 0     |
|  0.250000000  0.01                         | 0.01                            0.250000000   0.250000000 | 1     |
|  0.200000003  0.00110011001100110011001101 | 0.0011001100110011001101        0.200000048   0.200000003 | 0     |
|  0.166666672  0.00101010101010101010101011 | 0.00101010101010101010101       0.166666627   0.166666672 | 0     |
|  0.142857149  0.00100100100100100100100101 | 0.00100100100100100100101       0.142857194   0.142857149 | 0     |
|  0.125000000  0.001                        | 0.001                           0.125000000   0.125000000 | 1     |
|  0.111111112  0.000111000111000111000111001| 0.000111000111000111001         0.111111164   0.111111112 | 0     |
|  0.100000001  0.000110011001100110011001101| 0.00011001100110011001101       0.100000024   0.100000001 | 0     |
|  0.090909094  0.0001011101000101110100011  | 0.00010111010001011101001       0.090909123   0.090909094 | 0     |
|  0.083333336  0.000101010101010101010101011| 0.00010101010101010101011       0.083333373   0.083333336 | 0     |
|  0.076923080  0.000100111011000100111011001| 0.0001001110110001001111        0.076923132   0.076923080 | 0     |
|  0.071428575  0.000100100100100100100100101| 0.0001001001001001001001        0.071428537   0.071428575 | 0     |
|  0.066666670  0.000100010001000100010001001| 0.00010001000100010001001       0.066666722   0.066666670 | 0     |
|  0.062500000  0.0001                       | 0.0001                          0.062500000   0.062500000 | 1     |
```

```bash
$ make shufflePhi
icc -O3 -Wall -pedantic -std=c++11 -DDECIMAL -xmic-avx512 -fp-model strict summation.cpp -o shufflePhi && ./shufflePhi
 9999.99414062500000000000000000:  0.033300001 %
 9999.99511718750000000000000000:  0.624599993 %
 9999.99609375000000000000000000:  4.240699768 %
 9999.99707031250000000000000000: 15.360699654 %
 9999.99804687500000000000000000: 34.903400421 %
 9999.99902343750000000000000000: 33.413898468 %
10000.00000000000000000000000000: 11.187700272 %
10000.00097656250000000000000000:  0.235699996 %
```

### Discussion of Out-of-Order Execution

The results of shuffled summation, with zeros truncated, shows no difference
between the microarchitectures:

```output
--------------------------------  --------------------------------
         Xeon E5-1650                      Xeon Phi 7210          
--------------------------------  --------------------------------
 9999.9941406250:  0.033300000 %   9999.9941406250:  0.033300001 %
 9999.9951171875:  0.624600000 %   9999.9951171875:  0.624599993 %
 9999.9960937500:  4.240700000 %   9999.9960937500:  4.240699768 %
 9999.9970703125: 15.360700000 %   9999.9970703125: 15.360699654 %
 9999.9980468750: 34.903400000 %   9999.9980468750: 34.903400421 %
 9999.9990234375: 33.413900000 %   9999.9990234375: 33.413898468 %
10000.0000000000: 11.187700000 %  10000.0000000000: 11.187700272 %
10000.0009765625:  0.235700000 %  10000.0009765625:  0.235699996 %
--------------------------------  --------------------------------
```

## <a name="conclusions"></a>Conclusions

For both test cases — associativity of `a+b+c` and shuffled summations
equal to `10000` (decimal) and `128` (binary) — the programs in this
repository demonstrate that deviations from mathematically expected results
arise strictly due to the binary representation of floating-point numbers,
not computer hardware or CPU instruction sets. While this inadequacy of
[IEEE 754][eee] is nothing new, the source code in this repository does
provide simple, repeatable examples of the phenomenon, and may be of use
as a teaching aid.

## <a name="further-reading"></a>Further Reading

The following resources helped me to gain understanding.

1. David Goldberg (1991):
   [What Every Computer Scientist Should Know About Floating-Point Arithmetic][goldberg].

   Concise summary of how the standard floating-point representation
   works, and pitfalls to avoid.
2. Donald Knuth, *The Art of Computer Programming*, Vol. 2, Ed. 3,
   (1997):
   [§4.2.2: Accuracy of Floating Point Arithmetic][knuth]

   Philosophical review of floating-point representations with
   discussion of the compromises made to arrive at a working
   standard.
3. Erik Cheever (2001):
   [Representation of Numbers][cheever]

   Summary, with exercises, of how the standard represents real
   numbers.
4. Rick Regan (2012):
   [Why 0.1 Does Not Exist In Floating-Point][regan]

   Excellent illustration of why round-off error is a necessary evil
   in the standard representation.
5. James Demmel and Hong Diep Nguyen (2013):
   [Numerical Reproducibility and Accuracy at Exascale][demmel13]

   Short paper summarizing reproducibility of summation algorithms.
6. James Demmel, Hong Diep Nguyen, & Peter Ahrens (2015):
   [Cost of Floating-Point Reproducibility][demmel15]

   Detailed slides discussing sources of non-reproducibility and the
   computational cost of correcting the deficiencies.
7. Jeff Arnold (2017):
   [An Introduction to Floating-Point Arithmetic and Computation][arnold]

   Thorough slide deck highlighting problem areas for scientific
   computing.
8. Thomas Risse (2017):
   [Better is the Enemy of Good: Unums — An Alternative to IEEE 754 Floats and Doubles][risse]

   Concise discussion of an alternative floating-point
   representation.
9. James Matey (2018):
   [Re: Floating-Point Repeatability Issues on Modern Processors][matey]

   Insightful comment on the [GitHub Issue][git] with helpful
   references.

<!-- links -->

[doi]: https://doi.org/10.5281/zenodo.6524704
[dyn]: https://en.wikipedia.org/wiki/Out-of-order_execution
[eee]: https://en.wikipedia.org/wiki/IEEE_754
[fpm]: https://software.intel.com/en-us/node/522979
[fpu]: http://www.website.masmforum.com/tutorials/fptute/fpuchap1.htm#cword
[gcc]: https://gcc.gnu.org/
[git]: https://github.com/usnistgov/discuss/issues/8
[icc]: https://software.intel.com/c-compilers
[ico]: https://en.wikipedia.org/wiki/Core_(microarchitecture)
[ip6]: https://en.wikipedia.org/wiki/P6_(microarchitecture)
[isb]: https://en.wikipedia.org/wiki/Sandy_Bridge
[ism]: https://en.wikipedia.org/wiki/Silvermont
[jmt]: https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151
[lib]: https://en.wikipedia.org/wiki/List_of_arbitrary-precision_arithmetic_software
[mak]: https://www.gnu.org/software/make/
[mpf]: http://www.mpfr.org/
[mpm]: http://mpmath.org/
[ooe]: https://en.wikipedia.org/wiki/Out-of-order_execution
[rnd]: https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#680
[two]: https://en.wikipedia.org/wiki/Two%27s_complement
[zen]: https://zenodo.org/badge/latestdoi/135336811

<!-- references -->

[arnold]:   https://indico.cern.ch/event/626147/attachments/1456066/2247140/FloatingPoint.Handout.pdf
[cheever]:  http://www.swarthmore.edu/NatSci/echeeve1/Ref/BinaryMath/NumSys.html
[demmel13]: https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=6545912
[demmel15]: https://www.nist.gov/sites/default/files/documents/itl/ssd/is/NRE-2015-07-Nguyen_slides.pdf
[goldberg]: https://dl.acm.org/citation.cfm?id=103163
[knuth]:    https://books.google.com/books?id=Zu-HAwAAQBAJ&lpg=PT4&dq=knuth%20taocp%20vol%202&pg=PT385#v=onepage&q&f=true
[matey]:    https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151
[regan]:    http://www.exploringbinary.com/why-0-point-1-does-not-exist-in-floating-point/
[risse]:    https://ieeexplore.ieee.org/document/8080000/
