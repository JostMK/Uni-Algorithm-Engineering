//
// Created by Jost on 09/04/2025.
//

#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>

namespace exercise::one {
    struct Edge {
        int to;
        int weight;
    };

    class Graph {
    public:
        explicit Graph(std::fstream input_file);

        [[nodiscard]] int compute_weakly_connected_components() const;

    private:
        int m_node_count;

        std::vector<Edge> m_out_edges;
        std::vector<int> m_out_edges_offsets;

        std::vector<Edge> m_in_edges;
        std::vector<int> m_in_edges_offsets;
    };
} // exercise::one


#endif //GRAPH_H
