//
// Created by jostk on 05.05.2025.
//

#include <charconv>
#include <string>
#include <algorithm>
#include <functional>
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
    const auto level_start = line.find_last_of(' ');

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

    int from, to, weight, edgeIdA, edgeIdB;
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

static void calculate_offset_array_out_egdes(const std::vector<FMIEdge> &edges, std::vector<int> &offsets,
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
        if (const auto to = edges[i].from; last_to != to) {
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

    // group edges by source
    std::sort(edges.begin(), edges.end(),
              [](const auto &a, const auto &b) {
                  return a.from < b.from;
              });
    std::vector<int> original_edge_offsets;
    calculate_offset_array_out_egdes(edges, original_edge_offsets, node_count);

    // sorting nodes by level to improve cache locality
    std::sort(nodes.begin(), nodes.end(),
              [](const auto &a, const auto &b) {
                  return a.level < b.level;
              });
    m_node_index_map.resize(node_count);
    for (int i = 0; i < nodes.size(); ++i) {
        m_node_index_map[nodes[i].id] = i;
    }

    // create full graph
    {
        m_edges.resize(edges.size());
        m_edges_offsets.resize(node_count + 1);
        int edge_head = 0;
        for (int i = 0; i < nodes.size(); ++i) {
            m_edges_offsets[i] = edge_head;

            auto original_id = nodes[i].id;
            for (int j = original_edge_offsets[original_id]; j < original_edge_offsets[original_id + 1]; j++) {
                const auto edge = edges[j];
                // invalid replaced edge ids because they are not needed here
                // TODO: maybe calculate correct replaced edge indices, after sorting edges
                m_edges[edge_head++] = Edge{edge.to, edge.weight, -1, -1};
            }
        }
        m_edges_offsets[node_count] = edge_head;
    }

    // create up graph
    {
        int up_edge_head = 0;
        for (auto &[original_id, level]: nodes) {
            m_up_edges_offsets.push_back(up_edge_head);

            for (int j = original_edge_offsets[original_id]; j < original_edge_offsets[original_id + 1]; j++) {
                auto edge = edges[j];
                // invalid replaced edge ids because they are not needed here
                // TODO: maybe calculate correct replaced edge indices, after sorting edges
                if (nodes[edge.to].level > level) {
                    m_up_edges.push_back(Edge{edge.to, edge.weight, -1, -1});
                    up_edge_head++;
                }

            }
        }
        m_up_edges_offsets.push_back(up_edge_head);
    }

    // create down graph
    {
        // group edges by source
        std::sort(edges.begin(), edges.end(),
                  [](const auto &a, const auto &b) {
                      return a.to < b.to;
                  });
        std::vector<int> original_in_edge_offsets;
        calculate_offset_array_in_edges(edges, original_in_edge_offsets, node_count);

        int down_edge_head = 0;
        for (auto &[original_id, level]: nodes) {
            m_down_edges_offsets.push_back(down_edge_head);

            for (int j = original_in_edge_offsets[original_id]; j < original_in_edge_offsets[original_id + 1]; j++) {
                auto edge = edges[j];
                // invalid replaced edge ids because they are not needed here
                // TODO: maybe calculate correct replaced edge indices, after sorting edges
                if (nodes[edge.from].level > level) {
                    m_down_edges.push_back(Edge{edge.from, edge.weight, -1, -1});
                    down_edge_head++;
                }

            }
        }
        m_down_edges_offsets.push_back(down_edge_head);
    }
}
