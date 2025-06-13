compile_flags = -std=c++17 -O2 -s

run: exercise-3
	./exercise-3

e1: exercise-3
	./exercise-3 -e1

e2: exercise-3
	./exercise-3 -e2

exercise-3: main.o
	g++ $(compile_flags) main.o -o exercise-3

main.o: main.cpp Exercise1.h Exercise2.h Intersect.h InvertedIndex.h Stopwatch.h
	g++ $(compile_flags) -c main.cpp -o main.o

clean:
	rm -f main.o exercise-3
