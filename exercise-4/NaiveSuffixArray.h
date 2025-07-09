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
        std::vector<std::string> content;
    };

    class NaiveSuffixArray {
    public:
        explicit NaiveSuffixArray(std::ifstream data_file, int max_article_count = -1);

    private:
        std::vector<Article> m_Articles;

        std::string m_FullText;
        std::vector<size_t> m_Suffixes;
        std::vector<size_t> m_SuffixToArticleMap;
    };
} // Sheet4

#endif //NAIVESUFFIXARRAY_H
