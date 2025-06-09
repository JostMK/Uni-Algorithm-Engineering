//
// Created by jostk on 09.06.2025.
//

#pragma once

#include <cstdint>
#include <cmath>
#include <vector>

namespace Sheet3 {
    inline std::vector<uint32_t> intersect_naive(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
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

    inline std::vector<uint32_t> intersect_binary(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
        std::vector<uint32_t> result;
        result.reserve(v2.size());

        uint32_t lower_bound = 0;
        uint32_t upper_bound = v1.size() - 1;
        for (uint32_t b: v2) {
            uint32_t head = lower_bound;
            while (upper_bound - lower_bound >= 0) {
                head = (lower_bound + upper_bound) / 2;

                if (v1[head] == b) {
                    result.push_back(b);
                    break;
                }

                if (b < v1[head])
                    upper_bound = head - 1;
                else
                    lower_bound = head + 1;
            }

            lower_bound = head;
            upper_bound = v1.size() - 1;
        }

        return result;
    }

    inline std::vector<uint32_t> intersect_galloping(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
        if (v1.empty() || v2.empty())
            return {};

        std::vector<uint32_t> result;
        result.reserve(v2.size());

        uint32_t lower_bound = 0;
        uint32_t upper_bound = v1.size() - 1;
        bool galop = false;
        for (uint32_t b: v2) {
            if (galop) {
                uint32_t jump = lower_bound;
                for (int i = 0; v1[jump] < b; i++) {
                    jump = lower_bound + static_cast<uint32_t>(std::pow(2, i));

                    if (jump >= v1.size()) {
                        jump = v1.size() - 1;
                        break;
                    }
                }
                if (v1[jump] == b)
                    lower_bound = jump;
                upper_bound = jump;
            }

            uint32_t head = lower_bound;
            while (upper_bound - lower_bound >= 0) {
                head = (lower_bound + upper_bound) / 2;

                if (v1[head] == b) {
                    result.push_back(b);
                    break;
                }

                if (b < v1[head]) {
                    upper_bound = head - 1;
                } else {
                    lower_bound = head + 1;
                }
            }

            lower_bound = head;
            upper_bound = v1.size() - 1;
            galop = true;
        }

        return result;
    }
} // Sheet3