.PHONY: clear

compiler := clang++

fdebug := -g -O0 -fsanitize=address -fsanitize=undefined
frelease := -O3 -DNDEBUG

fcommon := -std=c++17 -Wall -Wextra -Wpedantic -Werror -Ilib
fchosen := ${fdebug}

all: mergesort karatsuba

mergesort:
	${compiler} ${fcommon} ${fchosen} src/mergesort.cpp -o bin/mergesort

karatsuba:
	${compiler} ${fcommon} ${fchosen} src/karatsuba.cpp -o bin/karatsuba

clear:
	rm mergesort karatsuba