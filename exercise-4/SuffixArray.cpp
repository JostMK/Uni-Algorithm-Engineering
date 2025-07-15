//
// Created by Jost on 09/07/2025.
//

#include "SuffixArray.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <set>
#include <cmath>

#include "Stopwatch.h"

namespace Sheet4 {
    SuffixArray::SuffixArray(std::ifstream data_file, const uint32_t max_article_count, const bool construct_naively) {
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

                if (m_Articles.size() >= max_article_count)
                    break;

                start_index = m_FullText.size();
                continue;
            }

            line = line.append(" ");
            m_FullText.append(line);

            for ([[maybe_unused]] char _c: line) {
                m_Suffixes.push_back(suffix_start_index++);
            }
        }
        // add end-of-text special character that compares the lowest to all other characters
        constexpr char end_of_text = static_cast<char>(3);
        m_FullText.append(&end_of_text);
        m_Suffixes.push_back(suffix_start_index);

        data_file.close();

        // create suffix array
        if (construct_naively) {
            sort_suffixes_naive();
        } else {
            sort_suffixes_iteratively();
        }

        // print stats
        std::cout << "Indexed " << m_FullText.size() << " characters" << std::endl;
    }

    std::vector<Article> SuffixArray::query(const std::string &substring) const {
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
                articles.insert(find_article_for_suffix(suffix));
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

            articles.insert(find_article_for_suffix(suffix));
            lower_bound--;
        }

        while (upper_bound < m_FullText.size()) {
            const auto suffix = m_Suffixes[upper_bound];
            const auto comp = std::char_traits<char>::compare(&m_FullText[suffix], &substring[0], substring.size());
            if (comp != 0)
                break;

            articles.insert(find_article_for_suffix(suffix));
            upper_bound--;
        }

        // create result vector
        std::vector<Article> result;
        result.reserve(m_Articles.size());
        for (const auto index: articles) {
            result.push_back(m_Articles[index]);
        }

        return result;
    }

    std::vector<Article> SuffixArray::naive_query(const std::string &substring) const {
        std::set<uint32_t> articles;

        auto pos = m_FullText.find(substring);
        while (pos != std::string::npos) {
            articles.insert(find_article_for_suffix(pos));
            pos = m_FullText.find(substring, pos + 1);
        }

        // create result vector
        std::vector<Article> result;
        result.reserve(m_Articles.size());
        for (const auto index: articles) {
            result.push_back(m_Articles[index]);
        }

        return result;
    }

    std::string SuffixArray::generate_preview(const std::vector<Article> &articles,
                                              const std::string &substring,
                                              const size_t max_article_count) const {
        std::string preview;
        for (int i = 0; i < std::min(articles.size(), max_article_count); ++i) {
            const auto &[start_index, end_index] = articles[i];
            const auto text = m_FullText.substr(start_index, end_index - start_index);

            // Get the first 5 words of the article
            size_t prefix_end = 0;
            for (int j = 0; j < 5; ++j) {
                const auto index = text.find(' ', prefix_end);
                if (index == std::string::npos)
                    break;
                prefix_end = index + 1;
            }

            // Get 2 words around query
            const auto query_index = text.find(substring);
            auto pre_face = query_index >= 2 ? query_index - 2 : query_index;
            auto post_face = query_index + substring.size() + 1;
            for (int j = 0; j < 2; ++j) {
                auto index = text.rfind(' ', pre_face);
                if (index != std::string::npos)
                    pre_face = index - 1;

                index = text.find(' ', post_face);
                if (index != std::string::npos)
                    post_face = index + 1;
            }

            if (prefix_end >= pre_face) {
                preview.append(text.substr(0, std::max(post_face, prefix_end - 1))).append("...\n");
            } else {
                preview.append(text.substr(0, prefix_end))
                        .append("...")
                        .append(text.substr(pre_face + 1, post_face - pre_face - 1))
                        .append("...\n");
            }
        }
        if (articles.size() > max_article_count) {
            const auto articles_count_left = articles.size() - max_article_count;
            preview.append("...and ").append(std::to_string(articles_count_left)).append(" more article(s).");
        }

        return preview;
    }

    void SuffixArray::sort_suffixes_naive() {
        const auto compair_suffixes = [&full_text = std::as_const(m_FullText)](const uint64_t a, const uint64_t b) {
            return std::char_traits<char>::compare(&full_text[a], &full_text[b], full_text.size() - std::max(a, b)) < 0;
        };

        std::sort(std::execution::par, m_Suffixes.begin(), m_Suffixes.end(), compair_suffixes);
    }

    void SuffixArray::sort_suffixes_iteratively() {
        std::vector<uint64_t> suffix_rank;
        suffix_rank.resize(m_FullText.size());
        std::fill(suffix_rank.begin(), suffix_rank.end(), 0);

        std::vector<uint64_t> suffix_rank_update_buffer;
        suffix_rank_update_buffer.resize(suffix_rank.size());

        // sort suffixes based on the first character
        {
            auto sw_ms = Stopwatch<std::chrono::milliseconds>::Start();
            const auto compair_suffixes = [&full_text = std::as_const(m_FullText)](const uint64_t a, const uint64_t b) {
                return full_text[a] < full_text[b];
            };
            std::sort(std::execution::par, m_Suffixes.begin(), m_Suffixes.end(), compair_suffixes);

            // initialize the rank based on the first character
            uint64_t counter = 0;
            suffix_rank[m_Suffixes[0]] = counter;
            for (uint64_t j = 1; j < m_Suffixes.size(); ++j) {
                if (m_FullText[m_Suffixes[j - 1]] == m_FullText[m_Suffixes[j]]) {
                    suffix_rank[m_Suffixes[j]] = counter;
                } else {
                    suffix_rank[m_Suffixes[j]] = ++counter;
                }
            }
            const auto time = sw_ms.Stop();
            std::cout << "[INFO] Setup iteration in " << time << "ms" << std::endl;
        }

        // for i=1..log(n) sort based on first 2^l characters
        auto sw_ms = Stopwatch<std::chrono::milliseconds>::Start();
        const auto iteration = std::ceil(std::log2(m_Suffixes.size()));
        for (int i = 0; i < iteration; ++i) {
            sw_ms.Restart();
            const auto half_length = 1 << i;

            // sort suffixes based on first half of character and then second half
            const auto compair_suffixes = [
                    &ranks = std::as_const(suffix_rank),
                    half_length
            ](const uint64_t a, const uint64_t b) {
                // if first half (computed in previous iteration) is not equal then this dictates order
                if (ranks[a] != ranks[b])
                    return ranks[a] < ranks[b];

                // else sort by second half order (also computed in previous interation because we work with suffixes)
                return ranks[a + half_length] < ranks[b + half_length];
            };
            std::sort(std::execution::par, m_Suffixes.begin(), m_Suffixes.end(), compair_suffixes);

            const auto split = sw_ms.Split();

            // update the rank based on the previous rank
            uint64_t counter = 0;
            suffix_rank_update_buffer[m_Suffixes[0]] = counter;
            for (uint64_t j = 1; j < m_Suffixes.size(); ++j) {
                if (suffix_rank[m_Suffixes[j - 1]] != suffix_rank[m_Suffixes[j]]) {
                    suffix_rank_update_buffer[m_Suffixes[j]] = ++counter;
                    continue;
                }

                if (suffix_rank[m_Suffixes[j - 1] + half_length] != suffix_rank[m_Suffixes[j] + half_length]) {
                    suffix_rank_update_buffer[m_Suffixes[j]] = ++counter;
                    continue;
                }

                suffix_rank_update_buffer[m_Suffixes[j]] = counter;
            }
            suffix_rank = suffix_rank_update_buffer;

            const auto time = sw_ms.Stop();
            std::cout << "[INFO] Iteration " << i << ": half length: " << half_length << "  Sorting: " << split
                      << "ms  Ranking: " << time << "ms  MaxRank:" << counter << std::endl;

            if (counter == m_Suffixes.size() - 1)
                break;
        }
    }

    uint32_t SuffixArray::find_article_for_suffix(uint64_t suffix) const {
        const auto index = std::lower_bound(
                m_Articles.begin(),
                m_Articles.end(),
                suffix,
                [](const Article &article, uint64_t value) {
                    return article.end_index < value;
                });

        return index - m_Articles.begin();
    }
} // Sheet4
