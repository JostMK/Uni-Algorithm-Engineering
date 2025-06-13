//
// Created by jostk on 10.06.2025.
//

#pragma once

#include "InvertedIndex.h"

namespace Sheet3 {
    inline void exercise_two(std::ifstream movies_data_file) {
        // Load movies from file:
        std::vector<Movie> movies;
        std::string line;
        while (std::getline(movies_data_file, line)) {
            auto tab_index = line.find('\t');
            auto title = line.substr(0, tab_index);
            auto description = line.substr(tab_index + 1, line.size() - tab_index - 1);
            movies.emplace_back(title, description);
        }
        movies_data_file.close();

        // Construct inverted index data structures:
        std::cout << "[BENCHMARK] Constructing inverted index data structures:" << std::endl;
        auto sw = Stopwatch<std::chrono::microseconds>::Start();

        std::stringstream construction_time_search_tree;
        sw.Restart();
        const auto inverted_index_st = InvertedIndexHashMap(movies);
        const auto search_tree_time = sw.Stop();
        construction_time_search_tree << std::fixed << std::setprecision(2) << search_tree_time << "us";

        std::stringstream construction_time_hashmap;
        sw.Restart();
        const auto inverted_index_hm = InvertedIndexHashMap(movies);
        const auto hashmap_time = sw.Stop();
        construction_time_hashmap << std::fixed << std::setprecision(2) << hashmap_time << "us";
        std::cout << "[BENCHMARK] "
                  << "Search Tree: " << std::left << std::setw(12) << construction_time_search_tree.str()
                  << "Hashmap: " << std::left << std::setw(12) << construction_time_hashmap.str()
                  << " indexing " << movies.size() << " movies.\n" << std::endl;

        // Benchmark query times:
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
            std::cout << "[BENCHMARK] Running benchmark for " << test.size() << " queries:" << std::endl;


            for (const auto &query: test) {
                std::stringstream naive_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index_hm.search(query);
                    const auto time = sw.Stop();
                    if (result.empty())
                        naive_time_str << "Failed";
                    else
                        naive_time_str << std::fixed << std::setprecision(2) << time << "us";
                }

                std::stringstream search_tree_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index_st.search(query);
                    const auto time = sw.Stop();
                    if (result.empty())
                        search_tree_time_str << "Failed";
                    else
                        search_tree_time_str << std::fixed << std::setprecision(2) << time << "us";
                }

                std::stringstream hashmap_time_str;
                {
                    sw.Restart();
                    const auto result = inverted_index_hm.search(query);
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
                          << " for \"" << query << "\"" << std::endl;
            }

        }

        // Interactive mode:
        std::cout << std::endl;
        std::cout << "[INFO] Interactive mode: Type in keywords to search them using the hashmap implementation.\n";
        std::cout << "[INFO] Interactive mode: Type [ENTER] to exit." << std::endl;
        while (true) {
            std::string input;
            std::getline(std::cin, input);

            if (input.empty())
                break;

            const auto result = inverted_index_hm.search(input);

            std::cout << "Found " << result.size() << " results:" << std::endl;
            for (int i = 0; i < result.size(); ++i) {
                const auto &movie = movies[result[i]];
                std::cout << i + 1 << ": " << movie.title << std::endl;
            }
        }
    }
}
