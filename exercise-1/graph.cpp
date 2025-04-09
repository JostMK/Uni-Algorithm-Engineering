//
// Created by Jost on 09/04/2025.
//

#include "graph.h"

#include <algorithm>
#include <charconv>
#include <iostream>

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

    static Edge parse_edge(const std::string_view line) {
        const auto from_end = line.find(' ');
        const auto to_start = from_end + 1;
        const auto to_end = line.find(' ', to_start);
        const auto weight_start = to_end + 1;
        const auto weight_end = line.find(' ', weight_start);

        int from, to, weight;
        std::from_chars(line.data(), line.data() + from_end, from);
        std::from_chars(line.data() + to_start, line.data() + to_end, to);
        std::from_chars(line.data() + weight_start, line.data() + weight_end, weight);

        return Edge{from, to, weight};
    }

    static int parse_file(std::fstream input_file, std::vector<Edge> &edges) {
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

    Graph::Graph(std::fstream input_file) {
        utils::Stopwatch sw;
        sw.Start();

        std::cout << "PARSING FILE" << std::endl;
        const auto node_count = parse_file(std::move(input_file), m_edges);
        sw.Split();

        std::cout << "SORTING EDGES" << std::endl;
        std::sort(m_edges.begin(), m_edges.end(),
                  [](const auto &a, const auto &b) {
                      return a.from < b.from;
                  });
        sw.Split();

        std::cout << "FINDING OFFSETS & ADDING EDGES" << std::endl;
        m_edges_offsets.resize(node_count + 1);
        int last_from = -1;
        for (int i = 0; i < m_edges.size(); ++i) {
            if (const auto &edge = m_edges[i]; last_from != edge.from) {
                for (int j = edge.from; j > last_from; --j) {
                    m_edges_offsets[j] = i;
                }
                last_from = edge.from;
            }
        }
        m_edges_offsets[node_count] = static_cast<int>(m_edges.size());
        sw.Stop();
    }
} // exercise::one
