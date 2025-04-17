#include <iostream>
#include <filesystem>
#include <fstream>

#include "graph.h"

namespace fs = std::filesystem;

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
    const auto graph = exercise::one::Graph(std::move(graph_file));

    // problem 2
    std::cout << "Calculating the amount of weak connectivity components:" << std::endl;
    const auto weak_component_count = graph.compute_weakly_connected_components();

    std::cout << "The graph has " << weak_component_count << " weakly connected components." << std::endl;

    // problem 4
    std::cout << "Calculating shortest path queries:" << std::endl;
    const auto distance = graph.compute_shortest_path_dijkstra(0, 1);
    std::cout << "Distance: " << distance << std::endl;

    return 0;
}
