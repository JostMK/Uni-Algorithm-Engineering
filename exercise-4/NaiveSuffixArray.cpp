//
// Created by Jost on 09/07/2025.
//

#include "NaiveSuffixArray.h"

#include <iostream>

namespace Sheet4 {
    NaiveSuffixArray::NaiveSuffixArray(std::ifstream data_file, const int max_article_count) {
        if (max_article_count > 0)
            m_Articles.reserve(max_article_count);

        int suffix_start_index = 0;
        Article article;
        std::string line;
        while (std::getline(data_file, line)) {
            if (line.empty()) {
                m_Articles.push_back(article);

                if (m_Articles.size() >= max_article_count)
                    break;

                article.content.clear();

                m_FullText.append("\n");
                m_Suffixes.push_back(suffix_start_index++);
                m_SuffixToArticleMap.push_back(m_Articles.size());

                continue;
            }

            line = line.append(" ");

            article.content.push_back(line);
            m_FullText.append(line);

            for (char c: line) {
                m_Suffixes.push_back(suffix_start_index++);
                m_SuffixToArticleMap.push_back(m_Articles.size());
            }
        }

        data_file.close();
    }
} // Sheet4
