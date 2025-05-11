//
// Created by jostk on 05.05.2025.
//

#include <iostream>
#include <filesystem>
#include "Stopwatch.h"
#include "ch_graph.h"

namespace fs = std::filesystem;

constexpr int CH_ITERATIONS = 1000;

const std::string default_graph_file_name = "graph.fmi";
const std::string default_query_file_name = "queries.txt";
const std::string default_result_file_name = "result.txt";

int main(const int argc, char *argv[]) {
    // try read-in CH graph from command line path provided by '-ch' option
    std::string ch_graph_file_name;
    if (argc > 2) {
        if (std::string(argv[1]) == "-ch") {
            ch_graph_file_name = argv[2];
        }
    }
    if (!ch_graph_file_name.empty()) {
        if (!fs::exists(ch_graph_file_name)) {
            std::cout << "Couldn't find '" << ch_graph_file_name << "' file!" << std::endl;
            return 1;
        }
        std::fstream graph_file(ch_graph_file_name, std::ios::in);
        if (!graph_file.is_open()) {
            std::cout << "Couldn't open '" << ch_graph_file_name << "' file!" << std::endl;
            return 1;
        }

        // problem 1
        std::cout << "Creating graph from file: '" << ch_graph_file_name << "'" << std::endl;
        auto sw = Stopwatch<std::chrono::microseconds>::Start();
        const auto graph = exercise::two::CHGraph(std::move(graph_file), true);
        std::cout << "Created in " << sw.Stop() / 1000 << "ms\n" << std::endl;

        std::cout << "Calculating " << CH_ITERATIONS << " shortest path queries:" << std::endl;
        long long overall_time = 0;
        for (int i = 0; i < CH_ITERATIONS; ++i) {
            const int start = std::rand() % graph.get_node_count();
            const int end = std::rand() % graph.get_node_count();

            sw.Restart();
            const auto dist = graph.compute_shortest_path(start, end);
            overall_time += sw.Split();
        }
        std::cout << "Computed in average of " << overall_time / CH_ITERATIONS << "us\n" << std::endl;
        return 0;
    }

    // generate CH graph from graph.fmi
    std::string graph_file_name = default_graph_file_name;
    if (argc > 1) {
        graph_file_name = argv[1];
    }
    if (!fs::exists(graph_file_name)) {
        std::cout << "Couldn't find '" << graph_file_name << "' file!" << std::endl;
        return 1;
    }
    std::fstream graph_file(graph_file_name, std::ios::in);
    if (!graph_file.is_open()) {
        std::cout << "Couldn't open '" << graph_file_name << "' file!" << std::endl;
        return 1;
    }

    std::cout << "Creating graph from file: '" << graph_file_name << "'" << std::endl;
    auto sw = Stopwatch<std::chrono::microseconds>::Start();
    const auto graph = exercise::two::CHGraph(std::move(graph_file), false);
    std::cout << "Created in " << sw.Stop() << "ms\n" << std::endl;

    return 0;
}
