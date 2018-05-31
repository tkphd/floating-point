all: std gmp
.PHONY: all

FLAGS = -O3 -Wall -pedantic

std: addition.c
	gcc $(FLAGS) $< -o $@ -lm && ./$@

phi: addition.c
	icc $(FLAGS) -xmic-avx512 -fp-model strict $< -o $@ && ./$@

gmp: addition.c
	gcc $(FLAGS) -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

unsafe: addition.c
	gcc $(FLAGS) -funsafe-math-optimizations $< -o $@ -lm && ./$@

shuffle: summation.cpp
	g++ $(FLAGS) -std=c++11 $< -o $@ && ./$@

shuffle10: summation.cpp
	g++ $(FLAGS) -std=c++11 -DDECIMAL $< -o $@ && ./$@

shufflePhi: summation.cpp
	icc $(FLAGS) -std=c++11 -DDECIMAL -xmic-avx512 -fp-model strict $< -o $@ && ./$@

.PHONY: clean
clean:
	rm -f std phi gmp unsafe shuffle shuffle10 shufflePhi
