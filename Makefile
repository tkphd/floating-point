all: std gmp
.PHONY: all

std: associative.c
	gcc -O3 -Wall $< -o $@ -lm && ./$@

gmp: associative.c
	gcc -O3 -Wall -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

unsafe: associative.c
	gcc -O3 -Wall -funsafe-math-optimizations $< -o $@ -lm && ./$@

.PHONY: clean
clean:
	rm -f std gmp unsafe
