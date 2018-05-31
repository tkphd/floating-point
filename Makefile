all: std gmp
.PHONY: all

FLAGS = -O3 -Wall -pedantic

std: associative.c
	gcc $(FLAGS) $< -o $@ -lm && ./$@

phi: associative.c
	icc $(FLAGS) -xmic-avx512 -fp-model strict $< -o $@ && ./$@

gmp: associative.c
	gcc $(FLAGS) -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

unsafe: associative.c
	gcc $(FLAGS) -funsafe-math-optimizations $< -o $@ -lm && ./$@

shuffle: shuffle-sum.cpp
	g++ $(FLAGS) -std=c++11 $< -o $@ && ./$@

shuffle10: shuffle-sum.cpp
	g++ $(FLAGS) -std=c++11 -DDECIMAL $< -o $@ && ./$@

shufflePhi: shuffle-sum.cpp
	icc $(FLAGS) -std=c++11 -DDECIMAL -xmic-avx512 -fp-model strict $< -o $@ && ./$@

.PHONY: clean
clean:
	rm -f std phi gmp unsafe shuffle shuffle10 shufflePhi
