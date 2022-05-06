all: std mpf
.PHONY: all

FLAGS = -O3 -Wall -pedantic

std: addition.c
	gcc $(FLAGS) $< -o $@ -lm && ./$@

phi: addition.c
	icc $(FLAGS) -xmic-avx512 -fp-model strict $< -o $@ && ./$@

spf: addition.c
	gcc $(FLAGS) -D_SINGLE_PRECISION_ -include "mpfr.h" $< -o $@ -lm -lmpfr && ./$@

mpf: addition.c
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
	rm -f std phi spf mpf unsafe shuffle shuffle10 shufflePhi
