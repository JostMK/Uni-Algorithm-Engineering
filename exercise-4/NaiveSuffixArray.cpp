//
// Created by Jost on 09/07/2025.
//

#include "NaiveSuffixArray.h"

#include <algorithm>
#include <iostream>
#include <set>

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

    std::vector<Article> NaiveSuffixArray::query(const std::string &substring) const {
        std::set<uint32_t> articles;

        // binary search the substring in the suffix array
        size_t lower_bound = 0;
        size_t upper_bound = m_FullText.size() - 1;
        while (lower_bound <= upper_bound) {
            const auto index = (lower_bound + upper_bound) / 2;
            const auto suffix = m_Suffixes[index];
            const auto comp = std::char_traits<char>::compare(&m_FullText[suffix], &substring[0], substring.size());

            if (comp == 0) {
                lower_bound = index - 1;
                upper_bound = index + 1;
                articles.insert(m_SuffixToArticleMap[suffix]);
                break;
            }

            if (comp < 0) {
                lower_bound = index + 1;
            } else {
                upper_bound = index - 1;
            }
        }

        // search for more hits to the left and right
        while (lower_bound > 0) {
            const auto suffix = m_Suffixes[lower_bound];
            const auto comp = std::char_traits<char>::compare(&m_FullText[suffix], &substring[0], substring.size());
            if (comp != 0)
                break;

            articles.insert(m_SuffixToArticleMap[suffix]);
            lower_bound--;
        }

        while (upper_bound < m_FullText.size()) {
            const auto suffix = m_Suffixes[upper_bound];
            const auto comp = std::char_traits<char>::compare(&m_FullText[suffix], &substring[0], substring.size());
            if (comp != 0)
                break;

            articles.insert(m_SuffixToArticleMap[suffix]);
            upper_bound--;
        }

        // create result vector
        std::vector<Article> result;
        for (const auto index: articles) {
            result.push_back(m_Articles[index]);
        }

        return result;
    }
} // Sheet4
