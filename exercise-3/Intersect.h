//
// Created by jostk on 09.06.2025.
//

#pragma once

#include <algorithm>
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

        auto lower = v1.begin();
        for (uint32_t b: v2) {
            auto it = std::lower_bound(lower, v1.end(), b);
            if (it != v1.end() && *it == b) {
                result.push_back(b);
                lower = it;
            }
        }

        return result;
    }

    // NOTE: couldn't get it to be faster than the std algorithm :(
    //inline std::vector<uint32_t> intersect_binary(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
    //    std::vector<uint32_t> result;
    //    result.reserve(v2.size());
    //
    //    uint32_t lower_bound = 0;
    //    uint32_t upper_bound = v1.size() - 1;
    //    for (uint32_t b: v2) {
    //        uint32_t head = lower_bound;
    //        while (upper_bound >= lower_bound) {
    //            // first divide then add to avoid overflows
    //            head = lower_bound / 2 + upper_bound / 2;
    //            if (lower_bound & 1 && upper_bound & 1) // correct error if both values are uneven
    //                head++;
    //
    //            if (v1[head] == b) {
    //                result.push_back(b);
    //                break;
    //            }
    //
    //            if (b < v1[head]) {
    //                // check numeric bounds and break early
    //                if (head == 0)
    //                    break;
    //
    //                upper_bound = head - 1;
    //            } else {
    //                // check numeric bounds and break early
    //                if (head == std::numeric_limits<uint32_t>::max())
    //                    break;
    //
    //                lower_bound = head + 1;
    //            }
    //        }
    //
    //        lower_bound = head;
    //        upper_bound = v1.size() - 1;
    //    }
    //
    //    return result;
    //}

    inline std::vector<uint32_t> intersect_galloping(const std::vector<uint32_t> &v1, const std::vector<uint32_t> &v2) {
        if (v1.empty() || v2.empty())
            return {};

        std::vector<uint32_t> result;
        result.reserve(v2.size());

        auto lower = v1.begin();
        auto upper = v1.end();
        // search first element using binary search on full array
        {
            const auto it = std::lower_bound(lower, upper, v2[0]);
            if (it != v1.end() && *it == v2[0]) {
                result.push_back(v2[0]);
                lower = it;
            }
        }

        // for remaining elements restrict binary search space using galloping
        for (int i = 1; i < v2.size(); ++i) {
            uint32_t b = v2[i];

            if (lower == v1.end())
                break;

            upper = lower;
            uint32_t jump = 1;
            upper += jump;
            while (upper != v1.end() && *upper < b) {
                jump <<= 1; // this is a little bit faster for bigger arrays, but has steps of 2^(i+1) - 1

                if (jump >= std::distance(upper, v1.end())) {
                    upper = v1.end();
                    break;
                }
                upper += jump;

                //jump <<= 1; // this would be true galloping with steps of 2^i
            }
            if (upper != v1.end() && *upper == b) {
                result.push_back(b);
                lower = upper;
                continue;
            }

            const auto it = std::lower_bound(lower, upper, b);
            if (it != v1.end() && *it == b) {
                result.push_back(b);
                lower = it;
            }
        }

        return result;
    }
} // Sheet3
