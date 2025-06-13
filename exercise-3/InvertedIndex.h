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

    class InvertedIndexHashMap {
    public:
        explicit InvertedIndexHashMap(const std::vector<Movie> &movies);

        std::vector<uint32_t> search(const std::string &query) const;

    private:
        std::unordered_map<std::string, std::vector<uint32_t> > m_Index;
    };
} // Sheet3
