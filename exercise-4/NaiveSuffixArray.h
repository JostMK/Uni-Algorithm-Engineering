//
// Created by Jost on 09/07/2025.
//

#ifndef NAIVESUFFIXARRAY_H
#define NAIVESUFFIXARRAY_H

#include <fstream>
#include <string>
#include <vector>

namespace Sheet4 {
    struct Article {
        uint64_t start_index;
        uint64_t end_index;
    };

    class NaiveSuffixArray {
    public:
        explicit NaiveSuffixArray(std::ifstream data_file, int max_article_count = -1);

        std::vector<Article> query(const std::string &substring) const;

    private:
        /// Stores the full text being indexed
        std::string m_FullText;
        /// Stores the index into m_FullText where the suffix begins
        std::vector<uint64_t> m_Suffixes; // needs to fit indices up to 6 billion
        /// Stores the index into m_Articles where the suffix (beginning at the accessed index) belongs to
        std::vector<uint32_t> m_SuffixToArticleMap; // needs to fit indices up to 4.3 million

        /// Stores the index of where the article begins and ends in m_FullText
        std::vector<Article> m_Articles;
    };
} // Sheet4

#endif //NAIVESUFFIXARRAY_H
