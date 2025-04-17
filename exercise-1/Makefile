run: exercise-1
	./exercise-1

exercise-1: main.o graph.o
	g++ main.o graph.o -o exercise-1

main.o: main.cpp graph.h Stopwatch.h
	g++ -c main.cpp -o main.o

graph.o: graph.cpp graph.h
	g++ -c graph.cpp -o graph.o

clean:
	rm -f main.o graph.o exercise-1 result.txt
