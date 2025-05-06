//
// Created by jostk on 05.05.2025.
//

#include <charconv>
#include <string>
#include <algorithm>
#include <queue>
#include "ch_graph.h"

enum READ_STATE {
    META,
    NODE_COUNT,
    EDGE_COUNT,
    NODES,
    EDGES,
    FINISHED
};

struct FMINode {
    int id;
    int level;
};

struct FMIEdge {
    int from, to;
    int weight;
    //int edgeIdA, edgeIdB;
};

static FMINode parse_node(const std::string_view line) {
    const auto id_end = line.find_first_of(' ');
    const auto level_start = line.find_last_of(' ') + 1;

    int id, level;
    std::from_chars(line.data(), line.data() + id_end, id);
    std::from_chars(line.data() + level_start, line.data() + line.length(), level);

    return FMINode{id, level};
}

static FMIEdge parse_edge(const std::string_view line) {
    const auto from_end = line.find(' ');
    const auto to_start = from_end + 1;
    const auto to_end = line.find(' ', to_start);
    const auto weight_start = to_end + 1;
    const auto weight_end = line.find(' ', weight_start);
    //const auto edgeIdA_end = line.find_last_of(' ');
    //const auto edgeIdA_start = line.find_last_of(' ', edgeIdA_end - 1);
    //const auto edgeIdB_start = edgeIdA_end + 1;

    //int from, to, weight, edgeIdA, edgeIdB;
    int from, to, weight;
    std::from_chars(line.data(), line.data() + from_end, from);
    std::from_chars(line.data() + to_start, line.data() + to_end, to);
    std::from_chars(line.data() + weight_start, line.data() + weight_end, weight);
    //std::from_chars(line.data() + edgeIdA_start, line.data() + edgeIdA_end, edgeIdA);
    //std::from_chars(line.data() + edgeIdB_start, line.data() + line.length(), edgeIdB);

    //return FMIEdge{from, to, weight, edgeIdA, edgeIdB};
    return FMIEdge{from, to, weight};
}

static int parse_file(std::fstream input_file, std::vector<FMINode> &nodes, std::vector<FMIEdge> &edges) {
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
                nodes.resize(node_count);
                state = READ_STATE::EDGE_COUNT;
                break;

            case READ_STATE::EDGE_COUNT:
                edges.resize(std::stoi(line));
                state = READ_STATE::NODES;
                break;

            case READ_STATE::NODES:
                nodes[node_index] = parse_node(line);
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

static void calculate_offset_array_out_edges(const std::vector<FMIEdge> &edges, std::vector<int> &offsets,
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

static void calculate_offset_array_in_edges(const std::vector<FMIEdge> &edges, std::vector<int> &offsets,
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

exercise::two::CHGraph::CHGraph(std::fstream input_file) {
    std::vector<FMINode> nodes;
    std::vector<FMIEdge> edges;
    const auto node_count = parse_file(std::move(input_file), nodes, edges);

    // sorting nodes by level to improve cache locality
    std::sort(nodes.begin(), nodes.end(),
              [](const auto &a, const auto &b) {
                  return a.level < b.level;
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

int exercise::two::CHGraph::compute_shortest_path(int source, int target) const {
    struct SPNode {
        int index;
        int distance;
        bool is_up_node;
    };

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
