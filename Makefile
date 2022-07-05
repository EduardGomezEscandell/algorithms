.PHONY: clear

compiler := clang++

debugflags := -g -O0 -fsanitize=address -fsanitize=undefined
releaseflags := -O3 -DNDEBUG
commonflags := -std=c++17 -Wall -Wextra -Wpedantic -Werror merge_sort.cpp

executable := bin/mergesort

debug:
	@echo "Compiling debug version"
	${compiler} ${debugflags} ${commonflags} -o ${executable}

release:
	@echo "Compiling release version"
	${compiler} ${releaseflags} ${commonflags} -o ${executable}

clear:
	rm ${executable}