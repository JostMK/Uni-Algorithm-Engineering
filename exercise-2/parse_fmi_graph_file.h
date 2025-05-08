//
// Created by Jost on 08/05/2025.
//

#ifndef PARSE_FMI_GRAPH_FILE_H
#define PARSE_FMI_GRAPH_FILE_H

#include <fstream>
#include <vector>

namespace exercise::two {
    struct FMIEdge {
        int from, to;
        int weight;
    };

    struct FMINode {
        int id;
        std::vector<FMIEdge> edges;
    };

    int parse_fmi_file(std::fstream input_file, std::vector<FMINode> &nodes);
}

#endif //PARSE_FMI_GRAPH_FILE_H
