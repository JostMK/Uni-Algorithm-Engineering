//
// Created by Jost on 06/06/2025.
//

#include <filesystem>
#include <fstream>

#include "Exercise1.h"
#include "Exercise2.h"

constexpr uint32_t DEFAULT_ELEMENT_COUNT = 1e8;
const std::string DEFAULT_DATA_FILE_PATH("movies.txt");

static void handle_exercise_one_input(const int argc, char *argv[]) {
    size_t max_size = std::numeric_limits<uint32_t>::max();
    {
        std::vector<uint32_t> dummy;
        max_size = std::min(max_size, dummy.max_size());
    }

    uint32_t element_count_a = DEFAULT_ELEMENT_COUNT;
    std::string input;

    if (argc > 2) {
        input = std::string(argv[2]);
    } else {
        std::cout << "Specify element count for intersect benchmark: [1-" << max_size << "]" << std::endl;
        std::getline(std::cin, input);
    }

    long long value;
    try {
        value = std::stoll(input);
    } catch (std::exception const &ex) {
        value = -1;
    }

    bool invalid = false;
    if (value <= 0) {
        std::cout << "[ERROR] Failed to parse argument, must be a number larger then zero."
                  << std::endl;
        invalid = true;
    }
    if (value > static_cast<uint32_t>(max_size)) {
        std::cout << "[ERROR] Failed to parse argument, number must be smaller then "
                  << max_size << "." << std::endl;
        invalid = true;
    }

    if (invalid) {
        std::cout << "[Info] Number of elements not specified! Using default value of "
                  << DEFAULT_ELEMENT_COUNT << "." << std::endl;
    } else {
        element_count_a = value;
    }

    Sheet3::exercise_one(element_count_a);
}

static void handle_exercise_two_input(const int argc, char *argv[]) {
    std::string data_file_path = DEFAULT_DATA_FILE_PATH;

    if (argc > 2) {
        data_file_path = std::string(argv[2]);
    } else {
        std::cout << "Specify path to movies data file: (" << DEFAULT_DATA_FILE_PATH << ")" << std::endl;
        std::string input;
        std::getline(std::cin, input);

        if (input.empty()) {
            std::cout << "[Info] Provided path empty! Using default path: \""
                      << DEFAULT_DATA_FILE_PATH << "\"." << std::endl;
        } else {
            data_file_path = input;
        }
    }

    if (!std::filesystem::exists(data_file_path)) {
        std::cout << "[ERROR] Provided path does not exist: \"" << data_file_path
                  << "\". Exiting program!" << std::endl;
        return;
    }

    std::ifstream input_stream(data_file_path);
    if (!input_stream) {
        std::cout << "[ERROR] Failed to open file: \"" << data_file_path
                  << "\". Exiting program!" << std::endl;
        return;
    }

    Sheet3::exercise_two(std::move(input_stream));
}

int main(const int argc, char *argv[]) {
    std::string exercise;
    if (argc > 1) {
        exercise = std::string(argv[1]);
    } else {
        std::cout << "Specify problem to run: [1, 2]" << std::endl;
        std::getline(std::cin, exercise);
        exercise = "-e" + exercise;
    }

    if (exercise == "-e1") {
        handle_exercise_one_input(argc, argv);
    } else if (exercise == "-e2") {
        handle_exercise_two_input(argc, argv);
    } else {
        std::cout << "[Error] Invalid problem number supplied, must be one of [1, 2]." << std::endl;
    }

    return 0;
}
