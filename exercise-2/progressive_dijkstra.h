//
// Created by Jost on 10/05/2025.
//

#ifndef PROGRESSIVE_DIJKSTRA_H
#define PROGRESSIVE_DIJKSTRA_H

#include <limits>
#include <list>
#include <queue>

#include "parse_fmi_graph_file.h"

namespace exercise::two {
    class ProgressiveDijkstra {
    public:
        explicit ProgressiveDijkstra(const std::vector<CHBuildNode> &nodes);

        void set_source(int source_node);

        [[nodsicard]] int shortest_path_to(int target_node);

    private:
        struct DijkstraDistance {
            int value;
            bool final;
        };

        struct DijkstraNode {
            int index;
            int distance;

            constexpr bool operator>(const DijkstraNode &other) const {
                return distance > other.distance;
            }
        };

        const std::vector<CHBuildNode> &nodes;

        std::vector<DijkstraDistance> m_distances{};
        std::list<int> m_dirty_distances{};

        std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<> > m_queue;
        int m_source_node;

        const DijkstraDistance clear_distance = DijkstraDistance{std::numeric_limits<int>::max(), false};
    };
}

#endif //PROGRESSIVE_DIJKSTRA_H
