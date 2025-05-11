//
// Created by jostk on 05.05.2025.
//

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <list>
#include <queue>

#include "ch_graph.h"
#include "parse_ch_graph_file.h"
#include "parse_fmi_graph_file.h"
#include "progressive_dijkstra.h"

namespace exercise::two {
    CHGraph::CHGraph(std::fstream input_file, const bool is_ch_graph) {
        if (is_ch_graph) {
            read_in_ch_graph(std::move(input_file));
        } else {
            generate_ch_graph(std::move(input_file));
        }
    }

    int CHGraph::compute_shortest_path(int source, int target) const {
        struct SPNode {
            int index;
            int distance;
            bool is_up_node;
        };

        // invalid node index
        if (source >= m_node_index_map.size() || target >= m_node_index_map.size()) {
            return -1;
        }

        source = m_node_index_map[source];
        target = m_node_index_map[target];

        int valid_min_distance = std::numeric_limits<int>::max();

        std::vector<int> up_distances;
        std::vector<int> down_distances;
        up_distances.resize(m_node_index_map.size(), std::numeric_limits<int>::max());
        down_distances.resize(m_node_index_map.size(), std::numeric_limits<int>::max());

        // creates a min priority queue ordered by distance
        auto cmp = [](const SPNode &left, const SPNode &right) { return left.distance > right.distance; };
        std::priority_queue<SPNode, std::vector<SPNode>, decltype(cmp)> queue(cmp);

        up_distances[source] = 0;
        queue.push(SPNode{source, 0, true});
        down_distances[target] = 0;
        queue.push(SPNode{target, 0, false});

        while (!queue.empty()) {
            const auto [index, distance, is_up_node] = queue.top();
            queue.pop();

            if (is_up_node) {
                // old invalid entry, not removed for performance
                if (distance > up_distances[index]) {
                    continue;
                }

                // check if possible shortest is path found
                if (down_distances[index] != std::numeric_limits<int>::max()) {
                    valid_min_distance = std::min(valid_min_distance, distance + down_distances[index]);
                }
            } else {
                // old invalid entry, not removed for performance
                if (distance > down_distances[index]) {
                    continue;
                }

                // check if possible shortest is path found
                if (up_distances[index] != std::numeric_limits<int>::max()) {
                    valid_min_distance = std::min(valid_min_distance, distance + up_distances[index]);
                }
            }

            // no more possible expansions shorter than shortest found valid path -> found the shortest path
            if (distance > valid_min_distance) {
                break;
            }

            if (is_up_node) {
                for (int i = m_up_edges_offsets[index]; i < m_up_edges_offsets[index + 1]; ++i) {
                    const auto [neighbour_index, weight] = m_up_edges[i];
                    if (const auto new_distance = distance + weight; new_distance < up_distances[neighbour_index]) {
                        up_distances[neighbour_index] = new_distance;
                        queue.push(SPNode{neighbour_index, new_distance, true});
                    }
                }
            } else {
                for (int i = m_down_edges_offsets[index]; i < m_down_edges_offsets[index + 1]; ++i) {
                    const auto [neighbour_index, weight] = m_down_edges[i];
                    if (const auto new_distance = distance + weight; new_distance < down_distances[neighbour_index]) {
                        down_distances[neighbour_index] = new_distance;
                        queue.push(SPNode{neighbour_index, new_distance, false});
                    }
                }
            }
        }

        // no path exists return an invalid distance
        if (valid_min_distance == std::numeric_limits<int>::max())
            return -1;

        return valid_min_distance;
    }

    int CHGraph::get_node_count() const {
        return static_cast<int>(m_node_index_map.size());
    }

    static void calculate_offset_array_out_edges(const std::vector<CHEdge> &edges,
                                                 std::vector<int> &offsets,
                                                 const int node_count) {
        offsets.resize(node_count + 1);
        int last_from = -1;
        for (int i = 0; i < edges.size(); ++i) {
            if (const auto from = edges[i].from; last_from != from) {
                for (int j = from; j > last_from; --j) {
                    offsets[j] = i;
                }
                last_from = from;
            }
        }
        offsets[node_count] = static_cast<int>(edges.size());
    }

    static void calculate_offset_array_in_edges(const std::vector<CHEdge> &edges,
                                                std::vector<int> &offsets,
                                                const int node_count) {
        offsets.resize(node_count + 1);
        int last_to = -1;
        for (int i = 0; i < edges.size(); ++i) {
            if (const auto to = edges[i].to; last_to != to) {
                for (int j = to; j > last_to; --j) {
                    offsets[j] = i;
                }
                last_to = to;
            }
        }
        offsets[node_count] = static_cast<int>(edges.size());
    }

    void CHGraph::read_in_ch_graph(std::fstream input_file) {
        std::vector<CHNode> nodes;
        std::vector<CHEdge> edges;
        const auto node_count = parse_ch_file(std::move(input_file), nodes, edges);

        // sorting nodes descending by level to improve cache locality
        std::sort(nodes.begin(), nodes.end(),
                  [](const auto &a, const auto &b) {
                      return a.level > b.level;
                  });
        m_node_index_map.resize(node_count);
        for (int i = 0; i < nodes.size(); ++i) {
            m_node_index_map[nodes[i].id] = i;
        }

        // create up graph
        {
            // group edges by source
            std::sort(edges.begin(), edges.end(),
                      [](const auto &a, const auto &b) {
                          return a.from < b.from;
                      });
            std::vector<int> original_edge_offsets;
            calculate_offset_array_out_edges(edges, original_edge_offsets, node_count);

            m_up_edges.reserve(edges.size());
            m_up_edges_offsets.resize(node_count + 1);
            int up_edge_head = 0;
            for (int i = 0; i < nodes.size(); ++i) {
                const auto [id, level] = nodes[i];
                m_up_edges_offsets[i] = up_edge_head;

                for (int j = original_edge_offsets[id]; j < original_edge_offsets[id + 1]; j++) {
                    const auto edge = edges[j];
                    const auto target = m_node_index_map[edge.to];
                    if (nodes[target].level > level) {
                        m_up_edges.push_back(Edge{target, edge.weight});
                        up_edge_head++;
                    }
                }
            }
            m_up_edges_offsets[node_count] = up_edge_head;
        }

        // create down graph
        {
            // group edges by target
            std::sort(edges.begin(), edges.end(),
                      [](const auto &a, const auto &b) {
                          return a.to < b.to;
                      });
            std::vector<int> original_in_edge_offsets;
            calculate_offset_array_in_edges(edges, original_in_edge_offsets, node_count);

            m_down_edges.reserve(edges.size());
            m_down_edges_offsets.resize(node_count + 1);
            int down_edge_head = 0;
            for (int i = 0; i < nodes.size(); ++i) {
                const auto [id, level] = nodes[i];
                m_down_edges_offsets[i] = down_edge_head;

                for (int j = original_in_edge_offsets[id]; j < original_in_edge_offsets[id + 1]; j++) {
                    const auto edge = edges[j];
                    const auto source = m_node_index_map[edge.from];
                    if (nodes[source].level > level) {
                        m_down_edges.push_back(Edge{source, edge.weight});
                        down_edge_head++;
                    }
                }
            }
            m_down_edges_offsets[node_count] = down_edge_head;
        }
    }

    static std::list<FMIEdge> contract_node(const std::vector<CHBuildNode> &nodes,
                                            ProgressiveDijkstra &dijkstra,
                                            const int node_index) {
        std::list<FMIEdge> shortcuts;
        const auto &node = nodes[node_index];

        for (auto [in_node, in_edge]: node.in_edges) {
            dijkstra.set_source(in_node);

            for (auto [out_node, out_edge]: node.out_edges) {
                if (in_node == out_node)
                    continue;

                // check if shortest-path from in_node to out_node goes over this node
                const int distance = dijkstra.shortest_path_to(out_node);
                const int direct_distance = in_edge.weight + out_edge.weight;
                if (distance < direct_distance)
                    continue;

                // if yes then add shortcut
                shortcuts.push_back(FMIEdge{in_node, out_node, distance});
            }
        }
        return shortcuts;
    }

    static int preprocess_graph(std::vector<CHBuildNode> &nodes, int node_count, int edge_count) {
        // generate CH working graph
        std::vector<CHBuildNode> working_nodes;
        working_nodes.resize(nodes.size());
        for (auto &[id, level, in_edges, out_edges]: nodes) {
            auto copy = CHBuildNode{id, level};
            copy.in_edges.insert(in_edges.begin(), in_edges.end());
            copy.out_edges.insert(out_edges.begin(), out_edges.end());

            working_nodes[id] = copy;
        }

        ProgressiveDijkstra dijkstra{working_nodes};
        int shortcuts_added = 0;
        for (int i = 0; i < node_count; ++i) {
            // find possible shortcuts
            auto shortcuts = contract_node(working_nodes, dijkstra, i);

            // delete node in working graph by deleting all in_edges and out_edges
            for (auto [in_node, in_edge]: working_nodes[i].in_edges) {
                working_nodes[in_node].out_edges.erase(i);
            }
            for (auto [out_node, out_edge]: working_nodes[i].out_edges) {
                working_nodes[out_node].in_edges.erase(i);
            }

            // add shortcuts and level in main graph
            nodes[i].level = i;
            for (auto [from, to, weight]: shortcuts) {
                nodes[from].out_edges[to] = CHBuildEdge{to, weight};
                nodes[to].in_edges[from] = CHBuildEdge{from, weight};

                // and also to work graph
                working_nodes[from].out_edges[to] = CHBuildEdge{to, weight};
                working_nodes[to].in_edges[from] = CHBuildEdge{from, weight};
            }

            shortcuts_added += static_cast<int>(shortcuts.size());


            if (shortcuts_added >= edge_count) {
                std::cout << "[Progress: 100%] " << shortcuts_added << " shortcuts added" <<
                        std::endl;
                std::cout << "Contracted " << i << " out of " << node_count << " nodes" << std::endl;
                break;
            }

            if (i % 5000 == 1) {
                const auto progress_percent = std::round((1000.0 * shortcuts_added / static_cast<double>(edge_count))) /
                                              10;
                std::cout << "[Progress: " << progress_percent << "%] " << shortcuts_added << " shortcuts added" <<
                        std::endl;
            }
        }

        return edge_count + shortcuts_added;
    }

    void CHGraph::generate_ch_graph(std::fstream input_file) {
        // Read-in fmi file
        std::vector<CHBuildNode> nodes;
        auto edge_count = parse_fmi_file(std::move(input_file), nodes);
        const auto node_count = static_cast<int>(nodes.size());

        // Preprocess graph to ch graph
        edge_count = preprocess_graph(nodes, node_count, edge_count);

        // Create data structure for query
        // sorting nodes descending by level to improve cache locality
        std::sort(nodes.begin(), nodes.end(),
                  [](const auto &a, const auto &b) {
                      return a.level > b.level;
                  });
        m_node_index_map.resize(node_count);
        for (int i = 0; i < nodes.size(); ++i) {
            m_node_index_map[nodes[i].id] = i;
        }

        m_up_edges.resize(edge_count);
        m_down_edges.resize(edge_count);

        m_up_edges_offsets.resize(node_count + 1);
        m_down_edges_offsets.resize(node_count + 1);

        int up_edge_head = 0;
        int down_edge_head = 0;
        for (const auto &[id, level, in_edges, out_edges]: nodes) {
            const auto new_index = m_node_index_map[id];
            m_up_edges_offsets[new_index] = up_edge_head;
            m_down_edges_offsets[new_index] = down_edge_head;

            for (const auto &[out_node, out_edge]: out_edges) {
                const auto new_out_id = m_node_index_map[out_node];
                if (nodes[new_out_id].level > level) {
                    m_up_edges[up_edge_head++] = Edge{new_out_id, out_edge.weight};
                }
            }
            for (const auto &[in_node, in_edge]: in_edges) {
                const auto new_in_id = m_node_index_map[in_node];
                if (nodes[new_in_id].level > level) {
                    m_down_edges[down_edge_head++] = Edge{new_in_id, in_edge.weight};
                }
            }
        }
        m_up_edges_offsets[node_count] = edge_count;
        m_down_edges_offsets[node_count] = edge_count;
    }
}
