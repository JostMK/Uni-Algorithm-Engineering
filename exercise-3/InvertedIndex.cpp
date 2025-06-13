//
// Created by jostk on 11.06.2025.
//

#include <string>
#include <sstream>
#include "InvertedIndex.h"
#include "Intersect.h"

namespace Sheet3 {
    InvertedIndexHashMap::InvertedIndexHashMap(const std::vector<Movie> &movies) {
        for (uint32_t i = 0; i < movies.size(); ++i) {
            const auto &movie = movies[i];
            const auto line = normalize_line(movie.title + " " + movie.description);

            std::stringstream words_stream(line);
            std::string word;
            while (words_stream >> word) {
                auto entry = m_Index.find(word);
                if (entry == m_Index.end()) {
                    m_Index.emplace(word, std::vector<uint32_t>{i});
                } else {
                    auto &list = entry->second;
                    if (list.at(list.size() - 1) != i)
                        list.push_back(i);
                }
            }
        }
    }

    std::vector<uint32_t> InvertedIndexHashMap::search(const std::string &query) const {
        bool is_first = true;
        std::vector<uint32_t> results;

        std::stringstream words_stream(normalize_line(query));
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

            if (results.empty())
                return {};
        }

        return results;
    }

    InvertedIndexSearchTree::InvertedIndexSearchTree(const std::vector<Movie> &movies) {
        for (uint32_t i = 0; i < movies.size(); ++i) {
            const auto &movie = movies[i];
            const auto line = normalize_line(movie.title + " " + movie.description);

            std::stringstream words_stream(line);
            std::string word;
            while (words_stream >> word) {
                auto entry = m_Index.find(word);
                if (entry == m_Index.end()) {
                    m_Index.emplace(word, std::vector<uint32_t>{i});
                } else {
                    auto &list = entry->second;
                    if (list.at(list.size() - 1) != i)
                        list.push_back(i);
                }
            }
        }
    }

    std::vector<uint32_t> InvertedIndexSearchTree::search(const std::string &query) const {
        bool is_first = true;
        std::vector<uint32_t> results;

        std::stringstream words_stream(normalize_line(query));
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

            if (results.empty())
                return {};
        }

        return results;
    }
} // Sheet3
