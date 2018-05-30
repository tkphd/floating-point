# Associativity of Floating Point Arithmetic
This repository addresses [usnistgov/discuss#8][_git] by exploring
floating-point rounding errors through the lens of the associative
property of addition. The programs herein demonstrate that deviations
from mathematical precision are due to the standard representation
of floating-point numbers in a binary data type, in compliance with
[IEEE 754][_eee]. The evidence presented here does not support any
role of the CPU, instruction sets, or [dynamic execution][_dyn] in
the deviations.

### Table of Contents
1. [Three Term Addition][#three-term-addition]
2. [Shuffled Summation][#shuffled-summation]
3. [Conclusions][#conclusions]
4. [Further Reading][#further-reading]

## Basic Premise
Addition of real numbers is [associative][_add]: inserting parentheses
must not change the computed sum. For example,
`(1+2)+(3+4)+5 = (1+2)+3+(4+5) = 1+(2+3)+(4+5) = 15`.
The [two's complement][_two] representation of integers naturally
supports associativity, within the representable range of `2^(n-1)-1`
for an `n`-bit representation. However, fractional real numbers incur
a round-off error since infinite digits cannot practically be stored.
The chief exception to this general rule is that real fractions formed
by the sum of powers of two *are* exactly represented, as a natural
consequence of the data format. It should be noted that two's complement
is *not* the internal representation used for floating point numbers,
but can be used to visualize floating point representations.

## Dependencies
- A C compiler, *e.g.* [GNU Compiler Collection][_gcc]
- Standard C libraries
- GNU [Make][_mak]
- [MPFR][_gnu] library headers and runtime, version 4 or greater
  - On Debian derivatives, `$ apt-get install libmpfr4 libmpfr-dev`

# <a name="three-term-addition"></a>Three Term Addition
When addition is associative, we expect the sum of three terms, `a+b+c`,
to be independent of computation as `(a+b)+c` or `a+(b+c)`. Due to the
binary representation of floating-point numbers ([IEEE 754][_eee]), *or* due
to out-of-order execution on some CPUs, this will not be the case for numerical
approximations to floating-point summation.

## Usage and Results
The code has three variants: `std` (standard representation), `gmp` (GNU MP
representation), and `unsafe` (`std` with unsafe optimizations). Use the
included Makefile or the commands in the following sections to compile the
executables.

Each program will output the value of `a` in decimal and binary form, `a+b+c`
in binary form, and both of the associative expressions in decimal form. When
the expressions agree, a `1` is printed in the last column; otherwise, it will
be `0`. The binary form of a floating-point number is similar to an integer
binary, with each bit representing a power of two that decreases from `0` at
the decimal. Therefore,
```
1   = 2^( 0)  = 1.00
1/2 = 2^(-1)  = 0.10
1/4 = 2^(-2)  = 0.01
3/4 = 1/2+1/4 = 0.11
```

### Built-in floating point representation
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

### GNU MPFR floating-point representation
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

### Optimized floating-point representation
Per the [Using the GNU Compiler Collection(GCC) &sect;3.10][_gcc], `-funsafe-math-optimizations`
enables optimizations that
- assume both arguments and results are valid
- violate IEEE and ANSI standards
- change the floating-point unit [control word][_fpu]
- cause programs that rely on exact implementation of IEEE or ISO rules to fail
- may yield faster code for programs that do not require the guarantees of these specifications.

In other words, it is ill-advised for production code. However,
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

### Discussion
As [@JRMatey-NIST noted][_jmt], this is a straight-forward byproduct of the binary
representation of floating-point numbers. Exact representation is possible for integral
exponents of 2; any other number incurs [rounding error][_rnd]. This is good to know,
especially for codes that frequently increment large values by small amounts.

Since many scientific computing applications model diffusive processes (heat transfer,
mass diffusion, etc.), the effect is expected to be small: the perturbations caused
by round-off error will be smoothed out by the stable numerical scheme without any
additional effort on the part of the programmer.

# <a name="shuffled-summation"></a>Shuffled Summation
Ideally, the sequence of decimals (powers of 10)
```
     1         1       1
10×  /   + 9×  /  + 9× / + 9×1 + 9×10 + 9×100 + 9×1000 = 10000.
    1000      100      10
```
However, due to the same floating point representation problem, variations arise
from the order of summation. As a demonstration, this program will generate a vector
of 64 numbers (10 + 9*6), then for each of 1 million trials, the same vector
gets shuffled before summing. The histogram of values is then reported.
For additional details, see the [original thread][_git].

Similarly, the sequence of binaries (powers of 2)
```
   1      1      1      1
8× / + 8× / + 8× / + 9× / + 8×1 + 8×2 + 8×4 + 8×8 = 128.
   16     8      4      2
```
The program will generate a vector of 65 numbers (9 + 8*7) and,
for each of 1 million trials, shuffle the vector before summing.
Due to the exact representation of powers-of-two, only one result
(128.) is expected for all million shuffles.

## Usage and Results
There are two variants, `shuffle` and `shuffle10`, which can be built using the Makefile or the
command listed below.

### Floating point representation of decimal sequence `∑10ⁿ`
```bash
$ make shuffle10
g++ -O -Wall -pedantic -std=c++11 -DDECIMAL shuffle-sum.cpp -o shuffle && ./shuffle
 9999.99414062500000000000000000:  0.033300001 %
 9999.99511718750000000000000000:  0.624599993 %
 9999.99609375000000000000000000:  4.240699768 %
 9999.99707031250000000000000000: 15.360699654 %
 9999.99804687500000000000000000: 34.903400421 %
 9999.99902343750000000000000000: 33.413898468 %
10000.00000000000000000000000000: 11.187700272 %
10000.00097656250000000000000000:  0.235699996 %
```

### Floating point representation of binary sequence `∑2ⁿ`
```bash
$ make shuffle
g++ -O -Wall -pedantic -std=c++11 shuffle-sum.cpp -o shuffle && ./shuffle
128.00000000000000000000000000: 100.000000000 %
```

### Discussion
The sequence comprised exclusively of powers of 2 is represented exactly,
*i.e.* 1 million repetitions produce the same result, exactly 128., every
time. The sequence of powers of 10 is approximate, with the exact result
computed in only 12 % of the million repetitions.

# <a name="conclusions"></a>Conclusions
For both test cases — associativity of `a+b+c` and shuffled summations
equal to `10000` (decimal) and `128` (binary) — the programs in this
repository demonstrate that deviations from mathematically expected results
arise strictly due to the binary representation of floating-point numbers,
not computer hardware or CPU instruction sets. While this inadequacy of
[IEEE 754][_eee] is nothing new, the source code in this repository does
provide simple, repeatable examples of the phenomenon, and may be of use
as a teaching aid.

If strict adherence to mathematical law is required, use a [high-precision math library][_lib].
For example, instead of the built-in data types, use
- [MPFR][_mpf] for C/C++.
- [MPMath][_mpm] for Python.

# <a name="further-reading"></a>Further Reading
1. David Goldberg, [What Every Computer Scientist Should Know About Floating-Point Arithmetic](https://dl.acm.org/citation.cfm?id=103163) (1991).
   Concise summary of how the standard floating-point representation works, and pitfalls to avoid.
2. Donald Knuth, [The Art of Computer Programming, Vol. 2, Ed. 3, &sect;4.2.2: Accuracy of Floating Point Arithmetic](https://books.google.com/books?id=Zu-HAwAAQBAJ&lpg=PT4&dq=knuth%20taocp%20vol%202&pg=PT385#v=onepage&q&f=true) (1997).
   Philosophical review of floating-point representations with discussion of the compromises made to arrive at a working standard.
3. Erik Cheever, [Representation of Numbers](http://www.swarthmore.edu/NatSci/echeeve1/Ref/BinaryMath/NumSys.html) (2001).
   Summary, with exercises, of how the standard represents real numbers.
4. Rick Regan, [Why 0.1 Does Not Exist In Floating-Point](http://www.exploringbinary.com/why-0-point-1-does-not-exist-in-floating-point/) (2012).
   Excellent illustration of why round-off error is a necessary evil in the standard representation.
5. James Demmel and Hong Diep Nguyen, [Numerical Reproducibility and Accuracy at Exascale](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=6545912) (2013).
   Short paper summarizing reproducibility of summation algorithms.
6. James Demmel, Hong Diep Nguyen, & Peter Ahrens, [Cost of Floating-Point Reproducibility](https://www.nist.gov/sites/default/files/documents/itl/ssd/is/NRE-2015-07-Nguyen_slides.pdf) (2015).
   Detailed slides discussing sources of non-reproducibility and the computational cost of correcting the deficiencies.
7. Jeff Arnold, [An Introduction to Floating-Point Arithmetic and Computation](https://indico.cern.ch/event/626147/attachments/1456066/2247140/FloatingPoint.Handout.pdf) (2017).
   Thorough slide deck highlighting problem areas for scientific computing.
8. Thomas Risse, [Better is the Enemy of Good: Unums — An Alternative to IEEE 754 Floats and Doubles](https://ieeexplore.ieee.org/document/8080000/) (2017).
   Concise discussion of an alternative floating-point representation.
9. James Matey, [Re: Floating-Point Repeatability Issues on Modern Processors](https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151) (2018).

<!--References-->
[_dyn]: https://en.wikipedia.org/wiki/Out-of-order_execution
[_eee]: https://en.wikipedia.org/wiki/IEEE_754
[_gcc]: https://gcc.gnu.org/
[_git]: https://github.com/usnistgov/discuss/issues/8
[_gnu]: http://www.mpfr.org/
[_jmt]: https://github.com/usnistgov/discuss/issues/8#issuecomment-392554151
[_lib]: https://en.wikipedia.org/wiki/List_of_arbitrary-precision_arithmetic_software
[_mak]: https://www.gnu.org/software/make/
[_mpf]: http://www.mpfr.org/
[_mpm]: http://mpmath.org/
[_rnd]: https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html#680
[_two]: https://en.wikipedia.org/wiki/Two%27s_complement
