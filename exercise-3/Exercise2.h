//
// Created by jostk on 10.06.2025.
//

#pragma once

#include "InvertedIndex.h"

namespace Sheet3 {
    inline void exercise_two(std::ifstream movies_data_file) {
        auto inverted_index = InvertedIndex(std::move(movies_data_file));

        auto result = inverted_index.search("Zombies");

        for (auto r: result) {
            std::cout << r.title << std::endl;
        }
    }
}
