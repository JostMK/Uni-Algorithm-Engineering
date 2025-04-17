cmake_minimum_required(VERSION 3.30)
project(Uni_Algorithm_Engineering)

set(CMAKE_CXX_STANDARD 17)

add_executable(Exercise-1-P2
        exercise-1/main.cpp
        exercise-1/graph.cpp
        exercise-1/graph.h
        exercise-1/Stopwatch.h
)

add_custom_command(TARGET Exercise-1-P2 POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CMAKE_SOURCE_DIR}/exercise-1/graph.fmi ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/graph.fmi
        COMMENT "Copying graph file"
)