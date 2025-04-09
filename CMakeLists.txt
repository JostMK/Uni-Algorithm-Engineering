cmake_minimum_required(VERSION 3.30)
project(Uni_Algorithm_Engineering)

set(CMAKE_CXX_STANDARD 17)

add_executable(Exercise-1-P2
        exercise-1/connectivity.cpp
        exercise-1/graph.cpp
        exercise-1/graph.h
)

configure_file(exercise-1/graph.fmi graph.fmi COPYONLY)
