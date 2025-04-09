#include <iostream>
#include <filesystem>
#include <fstream>

#include "graph.h"

namespace fs = std::filesystem;

const std::string default_graph_file_name = "graph.fmi";

int main(const int argc, char *argv[]) {
    std::cout << "Calculating the amount of weak connectivity components:" << std::endl;

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

    auto graph = exercise::one::Graph(std::move(graph_file));

    return 0;
}
