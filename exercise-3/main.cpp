//
// Created by Jost on 06/06/2025.
//

#include "Exercise1.h"

constexpr uint32_t DEFAULT_ELEMENT_COUNT = 1e8;

int main(const int argc, char *argv[]) {
    uint32_t element_count_a = DEFAULT_ELEMENT_COUNT;
    if (argc != 2) {
        std::cout << "[WARNING] Number of elements not specified! Using default value of " << DEFAULT_ELEMENT_COUNT <<
                  "." << std::endl;
    } else {
        const long long value = std::stoll(argv[1]);
        if (value > std::numeric_limits<uint32_t>::max()) {
            std::cout <<
                      "[ERROR] Failed to parse argument, number must be smaller then " << std::numeric_limits<
                    uint32_t>::max() << "." << std::endl;
            return 1;
        }
        if (value < 0) {
            std::cout <<
                      "[ERROR] Failed to parse argument, number must be larger then zero." << std::endl;
            return 1;
        }
        element_count_a = value;
    }

    Sheet3::exercise_one(element_count_a);

    return 0;
}
