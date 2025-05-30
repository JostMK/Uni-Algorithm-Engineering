//
// Created by Jost on 08/05/2025.
//

#include <charconv>
#include <string>
#include "parse_fmi_graph_file.h"

namespace exercise::two {
    enum READ_STATE {
        META,
        NODE_COUNT,
        EDGE_COUNT,
        NODES,
        EDGES,
        FINISHED
    };

    static CHBuildNode parse_fmi_node(const std::string_view line) {
        const auto id_end = line.find_first_of(' ');

        int id;
        std::from_chars(line.data(), line.data() + id_end, id);

        return CHBuildNode{id, -1};
    }

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

    int parse_fmi_file(std::fstream input_file, std::vector<CHBuildNode> &nodes) {
        auto state = READ_STATE::META;
        int edge_count = 0;

        int node_index = 0;
        int edge_index = 0;
        std::string line;
        while (std::getline(input_file, line)) {
            switch (state) {
                case READ_STATE::META: {
                    // metadata section ends with an empty line
                    if (line.empty()) {
                        state = READ_STATE::NODE_COUNT;
                    }
                    break;
                }

                case READ_STATE::NODE_COUNT: {
                    nodes.resize(std::stoi(line));
                    state = READ_STATE::EDGE_COUNT;
                    break;
                }

                case READ_STATE::EDGE_COUNT: {
                    edge_count = std::stoi(line);
                    state = READ_STATE::NODES;
                    break;
                }

                case READ_STATE::NODES: {
                    const auto node = parse_fmi_node(line);
                    nodes[node.id] = node;

                    node_index++;

                    if (node_index >= nodes.size()) {
                        state = READ_STATE::EDGES;
                    }
                    break;
                }

                case READ_STATE::EDGES: {
                    auto [from, to, weight] = parse_edge(line);
                    nodes[from].out_edges.emplace_back(to, weight);
                    nodes[to].in_edges.emplace_back(from, weight);
                    edge_index++;
                    if (edge_index >= edge_count) {
                        state = READ_STATE::FINISHED;
                    }
                    break;
                }

                case READ_STATE::FINISHED: {
                    break;
                }
            }
        }
        input_file.close();

        return edge_count;
    }
}
