cmake_minimum_required(VERSION 3.30)
project(Uni_Algorithm_Engineering)

set(CMAKE_CXX_STANDARD 17)

add_executable(Exercise-1
        exercise-1/main.cpp
        exercise-1/graph.cpp
        exercise-1/graph.h
        exercise-1/Stopwatch.h
)

add_custom_command(TARGET Exercise-1 POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CMAKE_SOURCE_DIR}/exercise-1/graph.fmi ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/graph.fmi
        COMMENT "Copying graph file"
)
add_custom_command(TARGET Exercise-1 POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CMAKE_SOURCE_DIR}/exercise-1/queries.txt ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/queries.txt
        COMMENT "Copying queries file"
)


add_executable(Exercise-2
        exercise-2/main.cpp
        exercise-2/ch_graph.cpp
        exercise-2/ch_graph.h
        exercise-2/parse_ch_graph_file.cpp
        exercise-2/parse_ch_graph_file.h
        exercise-2/parse_fmi_graph_file.cpp
        exercise-2/parse_fmi_graph_file.h
        exercise-2/progressive_dijkstra.cpp
        exercise-2/progressive_dijkstra.h
)

add_custom_command(TARGET Exercise-2 POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${CMAKE_SOURCE_DIR}/exercise-2/graph.fmi ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/graph.fmi
        COMMENT "Copying graph file"
)


add_executable(Exercise-3
        exercise-3/main.cpp
        exercise-3/Exercise1.h
        exercise-3/Intersect.h
        exercise-3/Exercise2.h
)