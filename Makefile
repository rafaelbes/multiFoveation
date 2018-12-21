all:
	g++ teste.c -O3
benchmark:
	g++ benchmarkTempoFeature.c
	g++ benchmarkU.c
	./a.out
debug:
	g++ teste.c -DDEBUG=1 -DWRITEIMG=1 -O3
tests:
	./a.out 0
	./a.out 1
	./a.out 2
	./a.out 3
times:
	./a.out 0 | grep seconds
	./a.out 1 | grep seconds
	./a.out 2 | grep seconds
	./a.out 3 | grep seconds
