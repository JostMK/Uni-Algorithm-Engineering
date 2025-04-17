#include <iostream>
#include <filesystem>
#include <fstream>

#include "graph.h"
#include "Stopwatch.h"

namespace fs = std::filesystem;

constexpr int DIJKSTRA_ITERATIONS = 100;

const std::string default_graph_file_name = "graph.fmi";

int main(const int argc, char *argv[]) {
    std::string graph_file_name = default_graph_file_name;
    if (argc > 1) {
        graph_file_name = argv[1];
    }

    if (!fs::exists(graph_file_name)) {
        std::cout << "Couldn't find 'graph.fmi' file!" << std::endl;
        return 1;
    }

    std::fstream graph_file(graph_file_name, std::ios::in);
    if (!graph_file.is_open()) {
        std::cout << "Couldn't open 'graph.fmi' file!" << std::endl;
        return 1;
    }

    // problem 1
    std::cout << "Creating graph from file: " << graph_file_name << std::endl;
    auto sw = Stopwatch::Start();
    const auto graph = exercise::one::Graph(std::move(graph_file));
    std::cout << "Created in " << sw.Stop() << "ms\n" << std::endl;

    // problem 2
    std::cout << "Calculating the amount of weak connectivity components:" << std::endl;
    sw.Restart();
    const auto weak_component_count = graph.compute_weakly_connected_components();
    std::cout << "Computed in " << sw.Stop() << "ms" << std::endl;
    std::cout << "The graph has " << weak_component_count << " weakly connected components.\n" << std::endl;

    // problem 4
    std::cout << "Calculating " << DIJKSTRA_ITERATIONS << " shortest path queries:" << std::endl;
    long long overall_time = 0;
    for (int i = 0; i < DIJKSTRA_ITERATIONS; ++i) {
        const int start = std::rand() % graph.get_node_count();
        const int end = std::rand() % graph.get_node_count();

        sw.Restart();
        const auto dist = graph.compute_shortest_path_dijkstra(start, end);
        overall_time += sw.Split();
    }
    std::cout << "Computed in average of " << overall_time / DIJKSTRA_ITERATIONS << "ms" << std::endl;

    return 0;
}
