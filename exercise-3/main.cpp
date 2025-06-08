//
// Created by Jost on 06/06/2025.
//

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <random>
#include <set>
#include <sstream>

#include "Stopwatch.h"

constexpr uint32_t DEFAULT_ELEMENT_COUNT = 1e8;

static std::vector<uint32_t> intersect_naive(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
    std::vector<uint32_t> result;
    result.reserve(v2.size());

    uint32_t head_a = 0;
    for (uint32_t i: v2) {
        while (v1[head_a] < i) {
            head_a++;
            if (head_a == v1.size()) {
                break;
            }
        }
        if (head_a == v1.size()) {
            break;
        }

        if (v1[head_a] == i) {
            result.push_back(i);
        }
    }

    return result;
}

static std::vector<uint32_t> intersect_binary(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
    return {};
}

static std::vector<uint32_t> intersect_galopping(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
    return {};
}

static void fill_with_random_numbers(std::vector<uint32_t> &elements, const uint32_t count, const uint32_t max_value) {
    if (count == max_value) {
        elements.resize(count);
        for (uint32_t i = 0; i < max_value; i++) {
            elements[i] = i;
        }
        return;
    }

    std::vector<uint32_t> rand_values;
    rand_values.resize(count);

    std::mt19937 rand_alg{1337};
    std::uniform_int_distribution<uint32_t> distribution{0, max_value - 1};

    for (auto &rand_value: rand_values) {
        rand_value = distribution(rand_alg);
    }

    std::sort(rand_values.begin(), rand_values.end());

    elements.reserve(count);
    uint32_t head = 0;
    for (const uint32_t value: rand_values) {
        if (head == 0 || value > elements[head - 1]) {
            elements.push_back(value);
            head++;
        }
    }
}

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

    std::vector<uint32_t> elements_a(element_count_a);
    for (uint32_t i = 0; i < element_count_a; i++) {
        elements_a[i] = i;
    }

    auto sw = Stopwatch<std::chrono::milliseconds>::Start();
    const uint32_t element_count_b = std::floor(std::log2(element_count_a));
    for (uint32_t i = 0; i < element_count_b; i++) {
        const auto count = static_cast<uint32_t>(static_cast<long double>(element_count_a) / std::pow(2, i));
        std::vector<uint32_t> elements_b;
        fill_with_random_numbers(elements_b, count, element_count_a);

        sw.Restart();

        intersect_naive(elements_a, elements_b);
        const auto naive_time = sw.Split();
        std::stringstream naive_time_str;
        naive_time_str << std::fixed << std::setprecision(2) << naive_time << "ms";

        intersect_binary(elements_a, elements_b);
        const auto binary_time = sw.Split();
        std::stringstream binary_time_str;
        binary_time_str << std::fixed << std::setprecision(2) << binary_time << "ms";

        intersect_galopping(elements_a, elements_b);
        const auto galopping_time = sw.Stop();
        std::stringstream galopping_time_str;
        galopping_time_str << std::fixed << std::setprecision(2) << galopping_time << "ms";

        std::cout << "[BENCHMARK] "
                << "Naive: " << std::left << std::setw(12) << naive_time_str.str()
                << "Binary: " << std::left << std::setw(12) << binary_time_str.str()
                << "Galopping: " << std::left << std::setw(12) << galopping_time_str.str()
                << " for " << elements_b.size() << " Elements"
                << std::endl;
    }

    return 0;
}
