//
// Created by jostk on 10.06.2025.
//

#pragma once

#include "InvertedIndex.h"

namespace Sheet3 {
    inline void exercise_two(std::ifstream movies_data_file) {
        auto inverted_index = InvertedIndex(std::move(movies_data_file));

        // Benchmark:
        {
            std::vector<std::string> test{"Zombie",
                                          "Zombies",
                                          "Vampire",
                                          "Vampires",
                                          "Zombie Vampire",
                                          "is",
                                          "Shrek",
                                          "Shrek's"
            };
            std::cout << "[BENCHMARK] Running benchmark for " << test.size() << " queries." << std::endl;

            auto sw = Stopwatch<std::chrono::microseconds>::Start();
            for (const auto &query: test) {
                std::stringstream naive_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index.search(query);
                    const auto time = sw.Stop();
                    if (result.empty())
                        naive_time_str << "Failed";
                    else
                        naive_time_str << std::fixed << std::setprecision(2) << time << "us";
                }

                std::stringstream search_tree_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index.search(query);
                    const auto time = sw.Stop();
                    if (result.empty())
                        search_tree_time_str << "Failed";
                    else
                        search_tree_time_str << std::fixed << std::setprecision(2) << time << "us";
                }

                std::stringstream hashmap_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index.search(query);
                    const auto time = sw.Stop();
                    if (result.empty())
                        hashmap_time_str << "Failed";
                    else
                        hashmap_time_str << std::fixed << std::setprecision(2) << time << "us";
                }

                std::cout << "[BENCHMARK] "
                          << "Naive: " << std::left << std::setw(12) << naive_time_str.str()
                          << "Search Tree: " << std::left << std::setw(12) << search_tree_time_str.str()
                          << "Hashmap: " << std::left << std::setw(12) << hashmap_time_str.str()
                          << " for \"" << query << "\""
                          << std::endl;
            }

        }

        // Interactive:
        std::cout << std::endl;
        std::cout << "[INFO] Interactive mode: Type in keywords to search for." << std::endl;
        std::cout << "[INFO] Interactive mode: Type [ENTER] to exit." << std::endl;
        while (true) {
            std::string input;
            std::getline(std::cin, input);

            if (input.empty())
                break;

            const auto result = inverted_index.search(input);

            std::cout << "Found " << result.size() << " results:" << std::endl;
            for (int i = 0; i < result.size(); ++i) {
                std::cout << i + 1 << ": " << result[i].title << std::endl;
            }
        }
    }
}
