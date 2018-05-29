all: std gmp
.PHONY: all

std: associative.c
	gcc -O3 -Wall $< -o $@ -lm && ./$@

gmp: associative.c
	gcc -O3 -Wall -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

unsafe: associative.c
	gcc -O3 -Wall -funsafe-math-optimizations $< -o $@ -lm && ./$@

shuffle: shuffle-sum.cpp
	g++ -O -Wall -pedantic -std=c++11 $< -o $@ && ./$@

.PHONY: clean
clean:
	rm -f std gmp unsafe shuffle
