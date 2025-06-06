//
// Created by jostk on 05.05.2025.
//

#include "ch_graph.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <future>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <thread>
#include <unordered_set>

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

    static void contract_node(const std::vector<CHBuildNode> &nodes,
                              ProgressiveDijkstra &dijkstra,
                              const int node_index,
                              std::list<FMIEdge> &shortcuts) {
        const auto &node = nodes[node_index];

        for (auto [in_node, in_weight]: node.in_edges) {
            dijkstra.set_source(in_node);

            for (auto [out_node, out_weight]: node.out_edges) {
                if (in_node == out_node)
                    continue;

                // check if shortest-path from in_node to out_node goes over this node
                const int distance = dijkstra.shortest_path_to(out_node);
                const int direct_distance = in_weight + out_weight;
                if (distance < direct_distance)
                    continue;

                // if yes then add shortcut
                shortcuts.push_back(FMIEdge{in_node, out_node, distance});
            }
        }
    }

    static std::vector<int> find_independent_set(const std::vector<CHBuildNode> &nodes,
                                                 std::vector<int> &uncontracted_nodes) {
        std::unordered_set<int> independent_set;

        // sort nodes by upper bound for edge difference = shortcuts_added - edges_deleted
        std::sort(uncontracted_nodes.begin(), uncontracted_nodes.end(),
                  [&nodes](auto a, auto b) {
                      const CHBuildNode &nodeA = nodes[a];
                      const CHBuildNode &nodeB = nodes[b];
                      const int edgeDiffA = (static_cast<int>(nodeA.in_edges.size()) - 1) * (
                                                static_cast<int>(nodeA.out_edges.size()) - 1);
                      const int edgeDiffB = (static_cast<int>(nodeB.in_edges.size()) - 1) * (
                                                static_cast<int>(nodeB.out_edges.size()) - 1);
                      return edgeDiffA < edgeDiffB;
                  });

        for (const auto node_index: uncontracted_nodes) {
            const auto &node = nodes[node_index];
            bool independent = true;
            for (auto [in_node, _]: node.in_edges) {
                if (independent_set.find(in_node) != independent_set.end()) {
                    independent = false;
                    break;
                }
            }
            if (!independent) {
                continue;
            }

            for (auto [out_node, _]: node.out_edges) {
                if (independent_set.find(out_node) != independent_set.end()) {
                    independent = false;
                    break;
                }
            }
            if (!independent) {
                continue;
            }

            independent_set.emplace(node.id);
        }

        return std::vector<int>{std::begin(independent_set), std::end(independent_set)};
    }

    static int preprocess_graph(std::vector<CHBuildNode> &nodes, const int node_count, const int edge_count) {
        constexpr int PARALLEL_DIJKSTRAS = 16;

        std::vector<bool> contracted;
        contracted.resize(nodes.size(), false);
        std::vector<int> uncontracted_nodes{};
        uncontracted_nodes.reserve(nodes.size());
        for (const auto &node: nodes) {
            uncontracted_nodes.push_back(node.id);
        }
        auto independent_set_handle = std::async(std::launch::async, find_independent_set, std::ref(nodes),
                                                 std::ref(uncontracted_nodes));

        // generate CH working graph
        std::vector<CHBuildNode> working_nodes;
        working_nodes.resize(nodes.size());
        for (auto &[id, level, in_edges, out_edges]: nodes) {
            auto copy = CHBuildNode{id, level};
            copy.in_edges.insert(copy.in_edges.begin(), in_edges.begin(), in_edges.end());
            copy.out_edges.insert(copy.out_edges.begin(), out_edges.begin(), out_edges.end());

            working_nodes[id] = copy;
        }

        int level = 0;
        int shortcuts_added = 0;
        int nodes_contracted = 0;

        while (shortcuts_added < edge_count) {
            std::cout << "[Info] Finding independent set for level " << level << std::endl;
            auto independent_set = independent_set_handle.get();
            if (independent_set.empty())
                break;

            std::cout << "[Info] Contracting nodes of independent set with " << independent_set.size() <<
                    " nodes for level " << level << " in parallel" << std::endl;
            std::array<std::future<std::list<FMIEdge> >, PARALLEL_DIJKSTRAS> threads;
            const int nodes_per_thread = std::ceil(
                static_cast<double>(independent_set.size()) / static_cast<double>(threads.size()));
            for (int i = 0; i < threads.size(); ++i) {
                threads[i] = std::async(std::launch::async, [independent_set, working_nodes, nodes_per_thread, i]() {
                    ProgressiveDijkstra dijkstra{working_nodes};
                    std::list<FMIEdge> shortcuts;
                    const auto last_node = std::min(nodes_per_thread * (i + 1),
                                                    static_cast<int>(independent_set.size()));
                    for (int j = i * nodes_per_thread; j < last_node; ++j) {
                        contract_node(working_nodes, dijkstra, independent_set[i], shortcuts);
                    }
                    return shortcuts;
                });
            }

            // update remaining uncontracted nodes set
            std::vector<int> remaining_uncontracted_nodes{};
            remaining_uncontracted_nodes.reserve(uncontracted_nodes.size() - independent_set.size());
            for (const auto node_index: independent_set) {
                contracted[node_index] = true;
            }
            for (const auto node_index: uncontracted_nodes) {
                if (contracted[node_index])
                    continue;
                remaining_uncontracted_nodes.push_back(node_index);
            }

            // join threads
            std::list<FMIEdge> shortcuts;
            for (auto &thread: threads) {
                auto result = thread.get();
                shortcuts.insert(shortcuts.begin(), result.begin(), result.end());
            }

            // add shortcuts
            std::cout << "[Info] Adding " << shortcuts.size() << " shortcuts for level " << level << std::endl;
            for (auto &[from, to, weight]: shortcuts) {
                // add shortcuts to graph
                nodes[from].out_edges.emplace_back(to, weight);
                nodes[to].in_edges.emplace_back(from, weight);

                // and also to work graph
                working_nodes[from].out_edges.emplace_back(to, weight);
                working_nodes[to].in_edges.emplace_back(from, weight);
            }

            shortcuts_added += static_cast<int>(shortcuts.size());
            nodes_contracted += static_cast<int>(independent_set.size());

            if (remaining_uncontracted_nodes.empty()) {
                // break early to avoid unnecessary work
                for (const auto node: independent_set) {
                    nodes[node].level = level;
                }
                break;
            }

            if (shortcuts_added >= edge_count) {
                // break early to avoid unnecessary work
                for (const auto node: independent_set) {
                    nodes[node].level = level;
                }
                level++;
                // assign all uncontracted nodes to the top level
                for (const int node: uncontracted_nodes) {
                    nodes[node].level = level;
                }
                break;
            }

            // prepare next independent set
            uncontracted_nodes = remaining_uncontracted_nodes;
            independent_set_handle = std::async(std::launch::async, find_independent_set, nodes,
                                                std::ref(uncontracted_nodes));

            // delete node from working graph
            for (const auto node: independent_set) {
                // delete node in working graph by deleting all in_edges and out_edges
                for (auto [in_node, in_edge]: working_nodes[node].in_edges) {
                    working_nodes[in_node].remove_out_edge(node);
                }
                for (auto [out_node, out_edge]: working_nodes[node].out_edges) {
                    working_nodes[out_node].remove_in_edge(node);
                }
                // assign level
                nodes[node].level = level;
            }
            level++;

            const auto progress_percent = std::round((1000.0 * shortcuts_added) / edge_count) / 10.0;
            std::cout << "[Progress: " << progress_percent << "%] " << shortcuts_added << " shortcuts added" <<
                    std::endl;
        }

        const auto nodes_contracted_percent = std::round((1000.0 * nodes_contracted) / node_count) / 10.0;
        std::cout << "[Progress: 100%] " << shortcuts_added << " shortcuts added" << std::endl;
        std::cout << "[Info] Contracted " << nodes_contracted << " out of " << node_count << " nodes [" <<
                nodes_contracted_percent << "%]" << std::endl;

        return edge_count + shortcuts_added;
    }

    void CHGraph::generate_ch_graph(std::fstream input_file) {
        // Read-in fmi file
        std::vector<CHBuildNode> nodes;
        auto edge_count = parse_fmi_file(std::move(input_file), nodes);
        const auto node_count = static_cast<int>(nodes.size());

        // Preprocess graph to ch graph
        std::cout << "[Info] Preprocessing graph for CH" << std::endl;
        edge_count = preprocess_graph(nodes, node_count, edge_count);

        // Create data structure for query
        std::cout << "[Info] Generating query data structure" << std::endl;
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

            for (const auto &[out_node, out_weight]: out_edges) {
                const auto new_out_id = m_node_index_map[out_node];
                if (nodes[new_out_id].level >= level) {
                    m_up_edges[up_edge_head++] = Edge{new_out_id, out_weight};
                }
            }
            for (const auto &[in_node, in_weight]: in_edges) {
                const auto new_in_id = m_node_index_map[in_node];
                if (nodes[new_in_id].level >= level) {
                    m_down_edges[down_edge_head++] = Edge{new_in_id, in_weight};
                }
            }
        }
        m_up_edges_offsets[node_count] = edge_count;
        m_down_edges_offsets[node_count] = edge_count;
    }
}
