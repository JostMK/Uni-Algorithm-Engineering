//
// Created by jostk on 09.06.2025.
//

#pragma once

#include <cstdint>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <random>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Stopwatch.h"
#include "Intersect.h"

namespace Sheet3 {
    inline void exercise_one(uint32_t element_count_a) {
        std::cout << "[BENCHMARK] Running benchmark with " << element_count_a << " elements." << std::endl;

        std::vector<uint32_t> elements_a(element_count_a);
        for (uint32_t i = 0; i < element_count_a; i++) {
            elements_a[i] = i;
        }

        auto sw = Stopwatch<std::chrono::microseconds>::Start();
        const uint32_t element_count_b = std::floor(std::log2(element_count_a));
        for (uint32_t i = 0; i < element_count_b; i++) {
            const auto count = static_cast<uint32_t>(static_cast<long double>(element_count_a) / std::pow(2, i));
            std::vector<uint32_t> elements_b;
            std::sample(
                    elements_a.begin(),
                    elements_a.end(),
                    std::back_inserter(elements_b),
                    count,
                    std::mt19937{std::random_device{}()}
            );
            std::sort(elements_b.begin(), elements_b.end());

            std::stringstream naive_time_str;
            {
                sw.Restart();
                const auto naive_result = intersect_naive(elements_a, elements_b);
                const auto naive_time = sw.Split();
                if (naive_result.size() == elements_b.size())
                    naive_time_str << std::fixed << std::setprecision(2) << naive_time << "us";
                else
                    naive_time_str << "Failed";
            }

            std::stringstream binary_time_str;
            {
                sw.Restart();
                const auto binary_result = intersect_binary(elements_a, elements_b);
                const auto binary_time = sw.Split();
                if (binary_result.size() == elements_b.size())
                    binary_time_str << std::fixed << std::setprecision(2) << binary_time << "us";
                else
                    binary_time_str << "Failed";
            }

            std::stringstream galloping_time_str;
            {
                sw.Restart();
                const auto galloping_result = intersect_galloping(elements_a, elements_b);
                const auto galloping_time = sw.Stop();
                if (galloping_result.size() == elements_b.size())
                    galloping_time_str << std::fixed << std::setprecision(2) << galloping_time << "us";
                else
                    galloping_time_str << "Failed";
            }

            std::cout << "[BENCHMARK] "
                      << "Naive: " << std::left << std::setw(12) << naive_time_str.str()
                      << "Binary: " << std::left << std::setw(12) << binary_time_str.str()
                      << "Galloping: " << std::left << std::setw(12) << galloping_time_str.str()
                      << " for " << elements_b.size() << " Elements"
                      << std::endl;
        }
    }
} // Sheet3