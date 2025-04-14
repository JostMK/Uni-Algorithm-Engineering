//
// Created by Jost on 09/04/2025.
//

#include "graph.h"

#include <algorithm>
#include <charconv>
#include <functional>
#include <iostream>
#include <stack>

#include "Stopwatch.h"

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
                case READ_STATE::NODE_COUNT: {
                    node_count = std::stoi(line);
                    state = READ_STATE::EDGE_COUNT;
                }
                break;
                case READ_STATE::EDGE_COUNT: {
                    const auto edge_count = std::stoi(line);
                    edges.resize(edge_count);
                    state = READ_STATE::NODES;
                }
                break;
                case READ_STATE::NODES: {
                    node_index++;

                    if (node_index >= node_count) {
                        state = READ_STATE::EDGES;
                    }
                }
                break;
                case READ_STATE::EDGES: {
                    const auto fmi_edge = parse_edge(line);
                    edges[edge_index] = fmi_edge;
                    edge_index++;

                    if (edge_index >= edges.size()) {
                        state = READ_STATE::FINISHED;
                    }
                }
                break;
                case READ_STATE::FINISHED: {
                }
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
        utils::Stopwatch sw;
        sw.Start();

        std::cout << "Parsing file.." << std::endl;
        std::vector<FMIEdge> edges;
        m_node_count = parse_file(std::move(input_file), edges);
        sw.Split();

        std::cout << "Calculating out edges.." << std::endl;
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
        sw.Split();

        std::cout << "Calculating in edges.." << std::endl;
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
        sw.Stop();
    }

    int Graph::compute_weakly_connected_components() const {
        int component_count = 0;
        std::vector<int> node_components;
        node_components.resize(m_node_count, -1);

        std::vector<int> stack;
        stack.resize(m_node_count);
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
} // exercise::one
