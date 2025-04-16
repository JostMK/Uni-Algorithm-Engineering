//
// Created by Jost on 10/04/2025.
//

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>
#include <iostream>

namespace utils {
    class Stopwatch {
    public:
        Stopwatch() = default;

        void Start() {
            begin = std::chrono::steady_clock::now();
        }

        void Stop() {
            end = std::chrono::steady_clock::now();
            std::cout << "Measured " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() <<
                    "ms" << std::endl;
        }

        void Split() {
            end = std::chrono::steady_clock::now();
            std::cout << "Measured " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() <<
                    "ms" << std::endl;
            begin = std::chrono::steady_clock::now();
        }

    private:
        std::chrono::steady_clock::time_point begin;
        std::chrono::steady_clock::time_point end;
    };
} // utils

#endif //STOPWATCH_H
