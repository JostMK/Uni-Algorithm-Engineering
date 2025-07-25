//
// Created by Jost on 09/07/2025.
//

#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

namespace Sheet4 {
    struct Article {
        uint64_t start_index;
        uint64_t end_index;
    };

    class SuffixArray {
    public:
        explicit SuffixArray(std::ifstream data_file, uint32_t max_article_count = -1, bool construct_naively = false);

        std::vector<Article> query(const std::string &substring) const;

        std::vector<Article> naive_query(const std::string &substring) const;

        std::string generate_preview(const std::vector<Article> &articles, const std::string &substring,
                                     size_t max_article_count = 3) const;

    private:
        /// Stores the full text being indexed
        std::string m_FullText;
        /// Stores the index into m_FullText where the suffix begins
        std::vector<uint64_t> m_Suffixes; // needs to fit indices up to 6 billion

        /// Stores the index of where the article begins and ends in m_FullText
        std::vector<Article> m_Articles;

        void sort_suffixes_naive();
        void sort_suffixes_iteratively();

        uint32_t find_article_for_suffix(uint64_t suffix) const;
    };
} // Sheet4

#endif //SUFFIXARRAY_H
