//
// Created by Jost on 10/04/2025.
//

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

class Stopwatch {
public:
    static Stopwatch Start() {
        Stopwatch stopwatch;
        stopwatch.begin = std::chrono::steady_clock::now();
        return stopwatch;
    }

    long long Split() {
        end = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
        begin = std::chrono::steady_clock::now();
        return duration;
    }

    long long Stop() {
        end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

    void Restart() {
        begin = std::chrono::steady_clock::now();
    }

private:
    Stopwatch() = default;

    std::chrono::steady_clock::time_point begin{};
    std::chrono::steady_clock::time_point end{};
};


#endif //STOPWATCH_H
