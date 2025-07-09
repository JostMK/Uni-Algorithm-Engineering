//
// Created by Jost on 09/07/2025.
//

#include "NaiveSuffixArray.h"

#include <algorithm>
#include <iostream>

namespace Sheet4 {
    NaiveSuffixArray::NaiveSuffixArray(std::ifstream data_file, const int max_article_count) {
        if (max_article_count > 0)
            m_Articles.reserve(max_article_count);

        // read in data
        int suffix_start_index = 0;
        uint64_t start_index = 0;
        std::string line;
        while (std::getline(data_file, line)) {
            if (line.empty()) {
                m_Articles.push_back(Article{start_index, m_FullText.size()});

                m_FullText.append("\n");
                m_Suffixes.push_back(suffix_start_index++);
                m_SuffixToArticleMap.push_back(m_Articles.size());

                if (m_Articles.size() >= max_article_count)
                    break;

                start_index = m_FullText.size();
                continue;
            }

            line = line.append(" ");
            m_FullText.append(line);

            for ([[maybe_unused]] char _c: line) {
                m_Suffixes.push_back(suffix_start_index++);
                m_SuffixToArticleMap.push_back(m_Articles.size());
            }
        }

        data_file.close();

        // create suffix array
        const auto compair_suffixes = [&full_text = std::as_const(m_FullText)](const uint64_t a, const uint64_t b) {
            return std::char_traits<char>::compare(&full_text[a], &full_text[b], full_text.size() - std::max(a, b)) < 0;
        };

        std::sort(m_Suffixes.begin(), m_Suffixes.end(), compair_suffixes);

        // print stats
        std::cout << "Indexed " << m_FullText.size() << " characters" << std::endl;
    }
} // Sheet4
