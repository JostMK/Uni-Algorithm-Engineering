//
// Created by Jost on 09/07/2025.
//

#include <filesystem>
#include <fstream>
#include <iostream>

#include "NaiveSuffixArray.h"
#include "Stopwatch.h"

const std::string WIKI_FILE = "dewiki-20220201-clean.txt";
constexpr uint32_t DEFAULT_ARTICLE_COUNT = 100000;

int main(const int argc, char *argv[]) {
    // parse argc for line count
    auto article_count = DEFAULT_ARTICLE_COUNT;
    if (argc >= 2) {
        try {
            article_count = std::stoll(argv[1]);
        } catch ([[maybe_unused]] std::exception const &ex) {
            std::cout << "[ERROR] Failed to parse article count argument: '" << argv[1]
                    << "', expected a number greater than zero." << std::endl;
            std::cout << "[INFO] Using default value of "
                << DEFAULT_ARTICLE_COUNT << " for article count." << std::endl;
        }
    } else {
        std::cout << "[INFO] No article count argument passed. Using default value of "
                << DEFAULT_ARTICLE_COUNT << "." << std::endl;
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
    // output construction time
    {
        auto sw_min = Stopwatch<std::chrono::minutes>::Start();
        const Sheet4::NaiveSuffixArray naive_suffix_array(std::move(input_stream), article_count);
        const auto create = sw_min.Stop();
        std::cout << "Created naive suffix array in " << create << " minutes." << std::endl;

        auto sw_ms = Stopwatch<std::chrono::milliseconds>::Start();
        auto articles = naive_suffix_array.query("deutlich");
        const auto query = sw_ms.Stop();
        std::cout << "Queried naive suffix array in " << query << " ms." << std::endl;
    }

    // compute suffix array with recursive sorting
    // output construction time

    // allow querying articles
    // output query time + naive query time

    return 0;
}
