//
// Created by jostk on 05.05.2025.
//

#include <iostream>
#include <filesystem>
#include "../exercise-1/Stopwatch.h"
#include "ch_graph.h"

namespace fs = std::filesystem;

int main(const int argc, char *argv[]) {
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
        auto sw = Stopwatch::Start();
        const auto graph = exercise::two::CHGraph(std::move(graph_file));
        std::cout << "Created in " << sw.Stop() << "ms\n" << std::endl;

        auto targets = std::vector<int>{
                754742,
                754743,
                754744,
                754745,
                754746,
                754747,
                754748,
                754749,
                754750,
                754751
        };
        for (auto t: targets) {
            auto distance = graph.compute_shortest_path(377371, t);
            std::cout << "Distance from 377371 to " << t << ": " << distance << std::endl;
        }
    }

    //TODO: Test with direct neighbours: 304064 304063


}
