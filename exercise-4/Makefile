compile_flags = -std=c++17 -O2 -s

run: exercise-4
	./exercise-4 100000

exercise-4: main.o suffix.o
	g++ $(compile_flags) main.o suffix.o -o exercise-4 -ltbb

main.o: main.cpp SuffixArray.h Stopwatch.h
	g++ $(compile_flags) -c main.cpp -o main.o

suffix.o: SuffixArray.cpp SuffixArray.h
	g++ $(compile_flags) -c SuffixArray.cpp -o suffix.o

clean:
	rm -f main.o suffix.o exercise-4
