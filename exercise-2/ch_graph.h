//
// Created by jostk on 05.05.2025.
//

#ifndef UNI_ALGORITHM_ENGINEERING_CH_GRAPH_H
#define UNI_ALGORITHM_ENGINEERING_CH_GRAPH_H


#include <fstream>
#include <vector>

namespace exercise::two {
    struct Edge {
        int to;
        int weight;
    };

    class CHGraph {
    public:
        explicit CHGraph(std::fstream input_file, bool is_ch_graph);

        [[nodiscard]] int compute_shortest_path(int source, int target) const;

        [[nodiscard]] int get_node_count() const;

    private:
        std::vector<int> m_node_index_map;

        std::vector<Edge> m_up_edges;
        std::vector<int> m_up_edges_offsets;

        std::vector<Edge> m_down_edges;
        std::vector<int> m_down_edges_offsets;

        void read_in_ch_graph(std::fstream input_file);
        void generate_ch_graph(std::fstream input_file);
    };
} // exercise::one


#endif //UNI_ALGORITHM_ENGINEERING_CH_GRAPH_H
