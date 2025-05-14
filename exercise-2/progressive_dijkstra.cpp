//
// Created by Jost on 10/05/2025.
//

#include "progressive_dijkstra.h"

namespace exercise::two {
    ProgressiveDijkstra::ProgressiveDijkstra(const std::vector<CHBuildNode> &nodes): nodes{nodes}, m_source_node(-1) {
        m_distances.resize(nodes.size(), clear_distance);
    }

    void ProgressiveDijkstra::set_source(const int source_node) {
        if (source_node == m_source_node)
            return;

        while (!m_dirty_distances.empty()) {
            m_distances[m_dirty_distances.back()] = clear_distance;
            m_dirty_distances.pop_back();
        }

        m_queue = std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<> >{};
        m_source_node = source_node;

        m_distances[m_source_node] = DijkstraDistance{0, true};
        m_dirty_distances.emplace_back(m_source_node);

        m_queue.push(DijkstraNode{m_source_node, 0});
    }

    int ProgressiveDijkstra::shortest_path_to(const int target_node) {
        if (m_distances[target_node].final)
            return m_distances[target_node].value;

        while (!m_queue.empty()) {
            const auto [index, distance] = m_queue.top();
            m_queue.pop();

            // node expanded
            m_distances[index].final = true;

            // old invalid entry, not removed for performance
            if (distance > m_distances[index].value) {
                continue;
            }

            // target node expanded, shortest path found
            if (index == target_node) {
                break;
            }

            for (auto [neighbour, weight]: nodes[index].out_edges) {
                if (const auto new_distance = distance + weight;
                    new_distance < m_distances[neighbour].value) {
                    m_distances[neighbour].value = new_distance;
                    m_dirty_distances.emplace_back(neighbour);

                    m_queue.push(DijkstraNode{neighbour, new_distance});
                }
            }
        }

        // no path exists return an invalid distance
        if (!m_distances[target_node].final)
            return -1;

        return m_distances[target_node].value;
    }
}
