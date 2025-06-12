//
// Created by jostk on 11.06.2025.
//

#pragma once

#include <fstream>
#include <vector>
#include <unordered_map>

namespace Sheet3 {
    struct Movie {
        std::string title;
        std::string description;
    };

    class InvertedIndex {
    public:
        explicit InvertedIndex(std::ifstream movies_data_file);

        std::vector<Movie> search(const std::string &query);

    private:
        std::vector<Movie> m_Movies;
        std::unordered_map<std::string, std::vector<uint32_t> > m_Index;

        static std::string normalize_line(const std::string &line);
    };
} // Sheet3
