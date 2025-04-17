//
// Created by Jost on 09/04/2025.
//

#include "graph.h"

#include <algorithm>
#include <charconv>
#include <functional>
#include <limits>
#include <queue>
#include <string>

namespace exercise::one {
    enum READ_STATE {
        META,
        NODE_COUNT,
        EDGE_COUNT,
        NODES,
        EDGES,
        FINISHED
    };

    struct FMIEdge {
        int from, to;
        int weight;
    };

    static FMIEdge parse_edge(const std::string_view line) {
        const auto from_end = line.find(' ');
        const auto to_start = from_end + 1;
        const auto to_end = line.find(' ', to_start);
        const auto weight_start = to_end + 1;
        const auto weight_end = line.find(' ', weight_start);

        int from, to, weight;
        std::from_chars(line.data(), line.data() + from_end, from);
        std::from_chars(line.data() + to_start, line.data() + to_end, to);
        std::from_chars(line.data() + weight_start, line.data() + weight_end, weight);

        return FMIEdge{from, to, weight};
    }

    static int parse_file(std::fstream input_file, std::vector<FMIEdge> &edges) {
        int node_count = 0;

        auto state = READ_STATE::META;
        int node_index = 0;
        int edge_index = 0;

        std::string line;
        while (std::getline(input_file, line)) {
            switch (state) {
                case READ_STATE::META:
                    // metadata section ends with an empty line
                    if (line.empty()) {
                        state = READ_STATE::NODE_COUNT;
                    }
                    break;

                case READ_STATE::NODE_COUNT:
                    node_count = std::stoi(line);
                    state = READ_STATE::EDGE_COUNT;
                    break;

                case READ_STATE::EDGE_COUNT:
                    edges.resize(std::stoi(line));
                    state = READ_STATE::NODES;
                    break;

                case READ_STATE::NODES:
                    // skip parsing nodes
                    node_index++;
                    if (node_index >= node_count) {
                        state = READ_STATE::EDGES;
                    }
                    break;

                case READ_STATE::EDGES:
                    edges[edge_index] = parse_edge(line);
                    edge_index++;
                    if (edge_index >= edges.size()) {
                        state = READ_STATE::FINISHED;
                    }
                    break;

                case READ_STATE::FINISHED:
                    break;
            }
        }
        input_file.close();

        return node_count;
    }

    static void calculate_offset_array(const std::vector<FMIEdge> &edges, std::vector<int> &offsets,
                                       const int node_count, const std::function<int(const FMIEdge &)> &selector) {
        offsets.resize(node_count + 1);
        int last_from = -1;
        for (int i = 0; i < edges.size(); ++i) {
            if (const auto from = selector(edges[i]); last_from != from) {
                for (int j = from; j > last_from; --j) {
                    offsets[j] = i;
                }
                last_from = from;
            }
        }
        offsets[node_count] = static_cast<int>(edges.size());
    }

    Graph::Graph(std::fstream input_file) {
        std::vector<FMIEdge> edges;
        m_node_count = parse_file(std::move(input_file), edges);

        std::sort(edges.begin(), edges.end(),
                  [](const auto &a, const auto &b) {
                      return a.from < b.from;
                  });
        calculate_offset_array(edges, m_out_edges_offsets, m_node_count, [&](const auto &edge) { return edge.from; });

        m_out_edges.resize(edges.size());
        for (int i = 0; i < edges.size(); ++i) {
            const auto [_, to, weight] = edges[i];
            m_out_edges[i] = Edge{to, weight};
        }

        std::sort(edges.begin(), edges.end(),
                  [](const auto &a, const auto &b) {
                      return a.to < b.to;
                  });
        calculate_offset_array(edges, m_in_edges_offsets, m_node_count, [&](const auto &edge) { return edge.to; });

        m_in_edges.resize(edges.size());
        for (int i = 0; i < edges.size(); ++i) {
            const auto [from, _, weight] = edges[i];
            m_in_edges[i] = Edge{from, weight};
        }
    }

    int Graph::compute_weakly_connected_components() const {
        int component_count = 0;
        std::vector<int> node_components;
        node_components.resize(m_node_count, -1);

        std::vector<int> stack;
        stack.resize(m_out_edges.size() * 2);
        int stack_top = -1;

        int component_index = 0;
        for (int i = 0; i < m_node_count; ++i) {
            if (node_components[i] != -1) {
                continue;
            }

            stack[++stack_top] = i;
            while (stack_top >= 0) {
                const auto node = stack[stack_top--];

                if (node_components[node] != -1) {
                    continue;
                }

                node_components[node] = component_index;

                for (int j = m_out_edges_offsets[node]; j < m_out_edges_offsets[node + 1]; ++j) {
                    stack[++stack_top] = m_out_edges[j].to;
                }
                for (int j = m_in_edges_offsets[node]; j < m_in_edges_offsets[node + 1]; ++j) {
                    stack[++stack_top] = m_in_edges[j].to;
                }
            }
            component_count++;
            component_index++;
        }

        return component_count;
    }

    int Graph::compute_shortest_path_dijkstra(const int source, const int target) const {
        struct SPNode {
            int index;
            int distance;
        };

        std::vector<int> distances;
        distances.resize(m_node_count, std::numeric_limits<int>::max());

        // creates a min priority queue ordered by distance
        auto cmp = [](const SPNode &left, const SPNode &right) { return left.distance > right.distance; };
        std::priority_queue<SPNode, std::vector<SPNode>, decltype(cmp)> queue(cmp);

        distances[source] = 0;
        queue.push(SPNode{source, 0});

        while (!queue.empty()) {
            const auto [index, distance] = queue.top();
            queue.pop();

            // old invalid entry, not removed for performance
            if (distance > distances[index]) {
                continue;
            }

            // target node expanded, shortest path found
            if (index == target) {
                break;
            }

            for (int i = m_out_edges_offsets[index]; i < m_out_edges_offsets[index + 1]; ++i) {
                const auto [neighbour_index, weight] = m_out_edges[i];
                if (const auto new_distance = distance + weight; new_distance < distances[neighbour_index]) {
                    distances[neighbour_index] = new_distance;
                    queue.push(SPNode{neighbour_index, new_distance});
                }
            }
        }

        return distances[target];
    }

    int Graph::get_node_count() const {
        return m_node_count;
    }
} // exercise::one
