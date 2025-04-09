//
// Created by Jost on 09/04/2025.
//

#include "graph.h"

#include <algorithm>
#include <charconv>
#include <iostream>

namespace exercise::one {
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
        //int osm_id;
        double lng, lat;
        //int height;
    };

    static FMINode parse_node(const std::string_view line) {
        const auto index_end = line.find(' ');
        const auto lng_start = line.find(' ', index_end + 1) + 1;
        const auto lng_end = line.find(' ', lng_start);
        const auto lat_start = lng_end + 1;
        const auto lat_end = line.find(' ', lat_start);

        int index;
        double lat, lng;
        std::from_chars(line.data(), line.data() + index_end, index);
        std::from_chars(line.data() + lat_start, line.data() + lat_end, lat);
        std::from_chars(line.data() + lng_start, line.data() + lng_end, lng);

        return FMINode{index, lng, lat};
    }

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

    static void parse_file(std::fstream input_file, std::vector<FMINode> &fmi_nodes, std::vector<Edge> &edges) {
        auto state = READ_STATE::META;

        int node_index = 0;
        int edge_index = 0;

        std::string line;
        while (std::getline(input_file, line)) {
            switch (state) {
                case READ_STATE::META:
                    // ignore section
                    // metadata section ends with an empty line
                    if (line.empty()) {
                        state = READ_STATE::NODE_COUNT;
                    }
                    break;
                case READ_STATE::NODE_COUNT: {
                    const int node_count = std::stoi(line);
                    fmi_nodes.resize(node_count);
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
                    const auto fmi_node = parse_node(line);
                    fmi_nodes[node_index] = fmi_node;
                    node_index++;

                    if (node_index >= fmi_nodes.size()) {
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
    }

    Graph::Graph(std::fstream input_file) {
        std::vector<FMINode> fmi_nodes;

        std::cout << "PARSING FILE" << std::endl;
        parse_file(std::move(input_file), fmi_nodes, m_edges);

        std::cout << "SORTING NODES" << std::endl;
        std::sort(fmi_nodes.begin(), fmi_nodes.end(),
                  [](const auto &a, const auto &b) {
                      return a.id < b.id;
                  });

        std::cout << "SORTING EDGES" << std::endl;
        std::sort(m_edges.begin(), m_edges.end(),
                  [](const auto &a, const auto &b) {
                      return a.from < b.from;
                  });

        std::cout << "FINDING OFFSETS & ADDING EDGES" << std::endl;
        m_nodes.resize(fmi_nodes.size());
        for (int i = 0; i < fmi_nodes.size(); ++i) {
            const auto &node = fmi_nodes[i];
            m_nodes[i] = Node{node.lat, node.lng};
        }

        m_edges_offsets.resize(m_nodes.size() + 1);

        int last_from = -1;
        for (int i = 0; i < m_edges.size(); ++i) {
            if (const auto &edge = m_edges[i]; last_from != edge.from) {
                for (int j = edge.from; j > last_from; --j) {
                    m_edges_offsets[j] = i;
                }
                last_from = edge.from;
            }
        }
        m_edges_offsets[m_nodes.size()] = static_cast<int>(m_edges.size());
    }
} // exercise::one
