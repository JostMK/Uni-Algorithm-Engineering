//
// Created by Jost on 09/04/2025.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>

namespace exercise::one {
    struct Node {
        double lat, lng;
    };

    struct Edge {
        int from;
        int to;
        int weight;
    };

    class Graph {
    public:
        explicit Graph(std::fstream input_file);

    private:
        std::vector<Node> m_nodes;
        std::vector<Edge> m_edges;
        std::vector<int> m_edges_offsets;
    };
} // exercise::one


#endif //GRAPH_H
