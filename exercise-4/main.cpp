//
// Created by Jost on 09/07/2025.
//

#include <filesystem>
#include <fstream>
#include <iostream>

#include "NaiveSuffixArray.h"
#include "Stopwatch.h"

const std::string WIKI_FILE = "dewiki-20220201-clean.txt";

int main(int argc, char *argv[]) {
    // parse argc for line count

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
        auto sw = Stopwatch<std::chrono::minutes>::Start();
        Sheet4::NaiveSuffixArray naive_suffix_array(std::move(input_stream), 100000);
        const auto naive_time = sw.Stop();
        std::cout << "Created naive suffix array in " << naive_time << " minutes." << std::endl;
    }

    // compute suffix array with recursive sorting
    // output construction time

    // allow querying articles
    // output query time + naive query time

    return 0;
}
