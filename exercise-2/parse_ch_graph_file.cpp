//
// Created by Jost on 08/05/2025.
//

#include <charconv>
#include <string>
#include "parse_ch_graph_file.h"

namespace exercise::two {
    enum READ_STATE {
        META,
        NODE_COUNT,
        EDGE_COUNT,
        NODES,
        EDGES,
        FINISHED
    };

    static CHNode parse_ch_node(const std::string_view line) {
        const auto id_end = line.find_first_of(' ');
        const auto level_start = line.find_last_of(' ') + 1;

        int id, level;
        std::from_chars(line.data(), line.data() + id_end, id);
        std::from_chars(line.data() + level_start, line.data() + line.length(), level);

        return CHNode{id, level};
    }

    static CHEdge parse_edge(const std::string_view line) {
        const auto from_end = line.find(' ');
        const auto to_start = from_end + 1;
        const auto to_end = line.find(' ', to_start);
        const auto weight_start = to_end + 1;
        const auto weight_end = line.find(' ', weight_start);

        int from, to, weight;
        std::from_chars(line.data(), line.data() + from_end, from);
        std::from_chars(line.data() + to_start, line.data() + to_end, to);
        std::from_chars(line.data() + weight_start, line.data() + weight_end, weight);

        return CHEdge{from, to, weight};
    }

    static int parse_ch_file(std::fstream input_file, std::vector<CHNode> &nodes, std::vector<CHEdge> &edges) {
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
                    nodes.resize(std::stoi(line));
                    state = READ_STATE::EDGE_COUNT;
                    break;

                case READ_STATE::EDGE_COUNT:
                    edges.resize(std::stoi(line));
                    state = READ_STATE::NODES;
                    break;

                case READ_STATE::NODES:
                    nodes[node_index++] = parse_ch_node(line);
                    if (node_index >= nodes.size()) {
                        state = READ_STATE::EDGES;
                    }
                    break;

                case READ_STATE::EDGES:
                    edges[edge_index++] = parse_edge(line);
                    if (edge_index >= edges.size()) {
                        state = READ_STATE::FINISHED;
                    }
                    break;

                case READ_STATE::FINISHED:
                    break;
            }
        }
        input_file.close();

        return static_cast<int>(nodes.size());
    }
}
