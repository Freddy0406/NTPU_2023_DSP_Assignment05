INCLUDE = -I.

all:
	gcc -O3 -o main.exe main.c function.c -lm

test:
	./main.exe

clean:
	rm -f main.exe output.raw

.PHONY: test clean
