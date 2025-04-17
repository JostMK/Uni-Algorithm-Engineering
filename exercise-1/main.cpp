#include <charconv>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "graph.h"
#include "Stopwatch.h"

namespace fs = std::filesystem;

constexpr int DIJKSTRA_ITERATIONS = 100;

const std::string default_graph_file_name = "graph.fmi";
const std::string default_query_file_name = "queries.txt";
const std::string default_result_file_name = "result.txt";

static void handle_query_file(std::fstream query_file, std::fstream result_file, const exercise::one::Graph &graph) {
    auto sw = Stopwatch::Start();
    std::string line;
    while (std::getline(query_file, line)) {
        int start, target;
        const auto seperator = line.find(' ');
        std::from_chars(line.data(), line.data() + seperator, start);
        std::from_chars(line.data() + seperator + 1, line.data() + line.size(), target);

        sw.Restart();
        const auto dist = graph.compute_shortest_path_dijkstra(start, target);
        const auto time = sw.Split();

        result_file << start << " " << target << " " << dist << " " << time << "\n";
    }

    query_file.close();
    result_file.close();
}

int main(const int argc, char *argv[]) {
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

    // problem 1
    std::cout << "Creating graph from file: '" << graph_file_name << "'" << std::endl;
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
    std::cout << "Computed in average of " << overall_time / DIJKSTRA_ITERATIONS << "ms\n" << std::endl;

    // problem 5
    std::string query_file_name = default_query_file_name;
    if (argc > 2) {
        query_file_name = argv[2];
    }
    if (!fs::exists(query_file_name)) {
        std::cout << "Couldn't find '" << query_file_name << "' file!" << std::endl;
        return 1;
    }
    std::fstream query_file(query_file_name, std::ios::in);
    if (!query_file.is_open()) {
        std::cout << "Couldn't open '" << query_file_name << "' file!" << std::endl;
        return 1;
    }

    std::cout << "Processing queries from file: '" << query_file_name << "'" << std::endl;
    std::fstream result_file(default_result_file_name, std::ios::out);
    handle_query_file(std::move(query_file), std::move(result_file), graph);
    std::cout << "Results written to '" << default_result_file_name << "'.\n" << std::endl;

    return 0;
}
