//
// Created by jostk on 11.06.2025.
//

#include <string>
#include <sstream>
#include "InvertedIndex.h"
#include "Intersect.h"

namespace Sheet3 {
    InvertedIndex::InvertedIndex(std::ifstream movies_data_file) {
        std::string line;
        while (std::getline(movies_data_file, line)) {
            auto tab_index = line.find('\t');
            auto title = line.substr(0, tab_index);
            auto description = line.substr(tab_index + 1, line.size() - tab_index - 1);

            uint32_t index = m_Movies.size();
            m_Movies.push_back({title, description});

            std::stringstream words_stream(line);
            std::string word;
            while (words_stream >> word) {
                auto entry = m_Index.find(word);
                if (entry == m_Index.end()) {
                    m_Index.emplace(word, std::vector<uint32_t>{index});
                } else {
                    auto &list = entry->second;
                    if (list.at(list.size() - 1) != index)
                        list.push_back(index);
                }
            }
        }
        movies_data_file.close();
    }

    std::vector<Movie> InvertedIndex::search(const std::string &query) {
        bool is_first = true;
        std::vector<uint32_t> results;

        std::stringstream words_stream(query);
        std::string word;
        while (words_stream >> word) {
            if (is_first) {
                auto entry = m_Index.find(word);
                if (entry == m_Index.end())
                    return {};

                results = entry->second;
                is_first = false;
                continue;
            }

            auto entry = m_Index.find(word);
            if (entry == m_Index.end())
                return {};

            results = intersect_galloping(results, entry->second);

            if (results.size() == 0)
                return {};
        }

        std::vector<Movie> movies;
        movies.reserve(results.size());
        for (auto i: results) {
            movies.push_back(m_Movies[i]);
        }
        return movies;
    }
} // Sheet3