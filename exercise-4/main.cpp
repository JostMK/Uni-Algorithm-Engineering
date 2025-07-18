//
// Created by Jost on 09/07/2025.
//

#include <filesystem>
#include <fstream>
#include <iostream>

#include "SuffixArray.h"
#include "Stopwatch.h"

const std::string WIKI_FILE = "dewiki-20220201-clean.txt";
constexpr uint32_t DEFAULT_ARTICLE_LOAD_COUNT = 100000;
constexpr uint32_t DEFAULT_ARTICLE_DISPLAY_COUNT = 3;

int main(const int argc, char *argv[]) {
    // parse argc for line count
    auto article_count = DEFAULT_ARTICLE_LOAD_COUNT;
    if (argc >= 2) {
        try {
            article_count = std::stoll(argv[1]);
        } catch ([[maybe_unused]] std::exception const &ex) {
            std::cout << "[ERROR] Failed to parse article count argument: '" << argv[1]
                    << "', expected a number greater than zero." << std::endl;
            std::cout << "[INFO] Using default value of "
                    << DEFAULT_ARTICLE_LOAD_COUNT << " for article count." << std::endl;
        }
    } else {
        std::cout << "[INFO] No article count argument passed. Using default value of "
                << DEFAULT_ARTICLE_LOAD_COUNT << "." << std::endl;
    }

    // open data file
    if (!std::filesystem::exists(WIKI_FILE)) {
        std::cout << "[ERROR] Wiki file does not exist in current directory: \"" << WIKI_FILE
                << "\". Exiting program!" << std::endl;
        return 1;
    }

    std::ifstream input_stream(WIKI_FILE);
    if (!input_stream) {
        std::cout << "[ERROR] Failed to open wiki file: \"" << WIKI_FILE
                << "\". Exiting program!" << std::endl;
        return 1;
    }

    // compute suffix array with naive sorting
    auto sw_sec = Stopwatch<std::chrono::seconds>::Start();
    const Sheet4::SuffixArray suffix_array(std::move(input_stream), article_count, true);
    auto create_time = sw_sec.Stop();
    std::cout << "[BENCHMARK] Created naive sort suffix array in " << create_time << " seconds." << std::endl;

    // compute suffix array with iterative sorting
    // NOTE: works but is **very** slow -> for 10k articles: 18s per iteration -> 3min total
    // TODO: fix performance and re-enable
    //std::ifstream input_stream_it(WIKI_FILE);
    //sw_min.Restart();
    //const Sheet4::SuffixArray suffix_array(std::move(input_stream_it), article_count, false);
    //create_time = sw_min.Stop();
    //std::cout << "Created iterative sort suffix array in " << create_time << " minutes." << std::endl;

    // allow querying articles
    std::cout << std::endl;
    std::cout << "[INFO] Type in substring to search for using the suffix array.\n";
    std::cout << "[INFO] Type <ENTER> to exit." << std::endl;
    auto sw_ms = Stopwatch<std::chrono::milliseconds>::Start();
    while (true) {
        std::cout << "\n[INPUT] Search-string: " << std::endl;
        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
            break;

        sw_ms.Restart();
        const auto naive_query_articles = suffix_array.naive_query(input);
        const auto naive_query_time = sw_ms.Stop();
        std::cout << "[BENCHMARK] Queried naive in " << naive_query_time << " ms. (" << naive_query_articles.size() <<
                " results)"
                << std::endl;

        sw_ms.Restart();
        const auto articles = suffix_array.query(input);
        const auto query_time = sw_ms.Stop();
        std::cout << "[BENCHMARK] Queried suffix array in " << query_time << " ms. (" << articles.size() << " results)"
                <<
                std::endl;

        if (articles.empty()) {
            std::cout << "\n[INFO] No articles found containing: '" << input << "'\n" << std::endl;
            continue;
        }

        const auto preview = suffix_array.generate_preview(articles, input, DEFAULT_ARTICLE_DISPLAY_COUNT);
        std::cout << "\n[INFO] Results preview:\n" << preview << "\n" << std::endl;
    }

    return 0;
}
