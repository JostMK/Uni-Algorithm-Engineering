//
// Created by Jost on 08/05/2025.
//

#ifndef PARSE_FMI_GRAPH_FILE_H
#define PARSE_FMI_GRAPH_FILE_H

#include <fstream>
#include <unordered_map>
#include <vector>

namespace exercise::two {
    struct FMIEdge {
        int from, to;
        int weight;
    };

    struct CHBuildEdge {
        int neighbour{};
        int weight{};

        CHBuildEdge() = default;

        CHBuildEdge(const int neighbour, const int weight) : neighbour(neighbour), weight(weight) {
        }
    };

    struct CHBuildNode {
        int id{};
        int level{};
        std::list<CHBuildEdge> in_edges;
        std::list<CHBuildEdge> out_edges;

        CHBuildNode() = default;

        CHBuildNode(const int id, const int level) : id(id), level(level) {
        }

        void remove_in_edge(const int neighbour) {
            for (auto it = in_edges.begin(); it != in_edges.end(); ++it) {
                if (it->neighbour == neighbour) {
                    in_edges.erase(it);
                    break;
                }
            }
        }

        void remove_out_edge(const int neighbour) {
            for (auto it = out_edges.begin(); it != out_edges.end(); ++it) {
                if (it->neighbour == neighbour) {
                    out_edges.erase(it);
                    break;
                }
            }
        }
    };

    int parse_fmi_file(std::fstream input_file, std::vector<CHBuildNode> &nodes);
}

#endif //PARSE_FMI_GRAPH_FILE_H
