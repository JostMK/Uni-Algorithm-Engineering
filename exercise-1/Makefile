compile_flags = -O2 -s

run: exercise-1
	./exercise-1

exercise-1: main.o graph.o
	g++ $(compile_flags) main.o graph.o -o exercise-1

main.o: main.cpp graph.h Stopwatch.h
	g++ $(compile_flags) -c main.cpp -o main.o

graph.o: graph.cpp graph.h
	g++ $(compile_flags) -c graph.cpp -o graph.o

clean:
	rm -f main.o graph.o exercise-1 result.txt
