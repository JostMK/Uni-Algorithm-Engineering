compile_flags = -std=c++17 -O2 -s

run: exercise-2
	./exercise-2

exercise-2: main.o ch_graph.o progressive_dijkstra.o parse_ch_graph_file.o parse_fmi_graph_file.o
	g++ $(compile_flags) main.o ch_graph.o progressive_dijkstra.o parse_ch_graph_file.o parse_fmi_graph_file.o -o exercise-2

main.o: main.cpp Stopwatch.h ch_graph.h
	g++ $(compile_flags) -c main.cpp -o main.o

ch_graph.o: ch_graph.cpp ch_graph.h parse_ch_graph_file.h parse_fmi_graph_file.h progressive_dijkstra.h
	g++ $(compile_flags) -c graph.cpp -o graph.o

progressive_dijkstra.o: progressive_dijkstra.cpp progressive_dijkstra.h
	g++ $(compile_flags) -c progressive_dijkstra.cpp -o progressive_dijkstra.o

parse_ch_graph_file.o: parse_ch_graph_file.cpp parse_ch_graph_file.h
	g++ $(compile_flags) -c parse_ch_graph_file.cpp -o parse_ch_graph_file.o

parse_fmi_graph_file.o: parse_fmi_graph_file.cpp parse_fmi_graph_file.h
	g++ $(compile_flags) -c parse_fmi_graph_file.cpp -o parse_fmi_graph_file.o

clean:
	rm -f main.o ch_graph.o progressive_dijkstra.o parse_ch_graph_file.o parse_fmi_graph_file.o exercise-2 result.txt
