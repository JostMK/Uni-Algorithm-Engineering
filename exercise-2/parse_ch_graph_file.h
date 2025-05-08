//
// Created by Jost on 08/05/2025.
//

#ifndef PARSE_CH_GRAPH_FILE_H
#define PARSE_CH_GRAPH_FILE_H

#include <fstream>
#include <vector>

namespace exercise::two {
    struct CHNode {
        int id;
        int level;
    };

    struct CHEdge {
        int from, to;
        int weight;
    };

    static int parse_ch_file(std::fstream input_file, std::vector<CHNode> &nodes, std::vector<CHEdge> &edges);
}

#endif //PARSE_CH_GRAPH_FILE_H
