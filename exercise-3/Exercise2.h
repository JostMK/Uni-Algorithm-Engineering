//
// Created by jostk on 10.06.2025.
//

#pragma once

#include "InvertedIndex.h"

namespace Sheet3 {
    inline void exercise_two(std::ifstream movies_data_file) {
        auto inverted_index = InvertedIndex(std::move(movies_data_file));

        const auto result = inverted_index.search("Zombie Vampire");

        std::cout << "Found " << result.size() << " results:" << std::endl;
        for (const auto &[title, description]: result) {
            std::cout << title << std::endl;
            //std::cout << description << std::endl;
            //std::cout << std::endl;
        }
    }
}
