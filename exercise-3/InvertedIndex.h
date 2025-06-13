//
// Created by jostk on 11.06.2025.
//

#pragma once

#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>

namespace Sheet3 {
    struct Movie {
        std::string title;
        std::string description;

        Movie(std::string title, std::string description) : title(title), description(description) {}
    };

    inline std::string normalize_line(const std::string &line) {
        auto result = line;
        for (auto &c: result) {
            // Replace punctuation with whitespace
            // -> line gets split on whitespace characters into words
            // -> this ensures words like "Shrek." or "Shrek's" all get listed under "shrek"
            if (c == '.' || c == ',' || c == ',' || c == '?' || c == '!'
                || c == ':' || c == ';' || c == '\'' || c == '"' || c == '-' || c == '&') {
                c = ' ';
            }

            // Normalize to lower-case
            c = static_cast<char>(std::tolower(c));
        }

        return result;
    }

    // Not the cleanest solution but I don't know enough about templates to make this nice .-.
    // But having the exact same code twice just because the container changed also seemed wrong.
#define InvertedIndexSearchTree InvertedIndex<std::map<std::string, std::vector<uint32_t>>>
#define InvertedIndexHashmap InvertedIndex<std::unordered_map<std::string, std::vector<uint32_t>>>

    template<typename DATASTRUCT>
    class InvertedIndex {
    public:
        explicit InvertedIndex(const std::vector<Movie> &movies) {
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

        std::vector<uint32_t> search(const std::string &query) const {
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

    private:
        DATASTRUCT m_Index;
    };
} // Sheet3
