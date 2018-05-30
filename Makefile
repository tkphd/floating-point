all: std gmp
.PHONY: all

std: associative.c
	gcc -O3 -Wall $< -o $@ -lm && ./$@

phi: associative.c
	icc -O3 -xmic-avx512 -Wall $< -o $@ -qopt-prefetch=3 -qopt-report=4 -qopt-report-file="qopt-assoc.log" -lm && ./$@

gmp: associative.c
	gcc -O3 -Wall -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

unsafe: associative.c
	gcc -O3 -Wall -funsafe-math-optimizations $< -o $@ -lm && ./$@

shuffle: shuffle-sum.cpp
	g++ -O3 -Wall -pedantic -std=c++11 $< -o $@ && ./$@

shuffle10: shuffle-sum.cpp
	g++ -O3 -Wall -pedantic -std=c++11 -DDECIMAL $< -o $@ && ./$@

shufflePhi: shuffle-sum.cpp
	icc -O3 -Wall -pedantic -std=c++11 -xmic-avx512 $< -o $@ -qopt-prefetch=3 -qopt-report=4 -qopt-report-file="qopt-shuffle.log" && ./$@

.PHONY: clean
clean:
	rm -f std phi gmp unsafe shuffle shuffle10 shufflePhi
