//
// Created by Jost on 08/05/2025.
//

#ifndef PARSE_FMI_GRAPH_FILE_H
#define PARSE_FMI_GRAPH_FILE_H

#include <fstream>
#include <unordered_map>
#include <vector>

namespace exercise::two {
    struct FMIEdge {
        int from, to;
        int weight;
    };

    struct CHBuildEdge {
        int neighbour;
        int weight;
    };

    struct CHBuildNode {
        int id;
        int level;
        std::unordered_map<int, CHBuildEdge> in_edges;
        std::unordered_map<int, CHBuildEdge> out_edges;
    };

    int parse_fmi_file(std::fstream input_file, std::vector<CHBuildNode> &nodes);
}

#endif //PARSE_FMI_GRAPH_FILE_H
