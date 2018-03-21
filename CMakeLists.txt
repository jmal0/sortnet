cmake_minimum_required(VERSION 2.8)
project (sortnet CXX)

if (NOT CXX_STD)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CXX_STD}")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (NOT EXTRA_COMPILE_FLAGS)
    set(EXTRA_COMPILE_FLAGS "-march=native")
endif()

add_executable(test_sortnet test_sortnet.cpp)
target_include_directories(test_sortnet
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(test_sortnet
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        ${EXTRA_COMPILE_FLAGS}
)

add_executable(benchmark_sortnet benchmark_sortnet.cpp)
target_include_directories(benchmark_sortnet
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(benchmark_sortnet
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        ${EXTRA_COMPILE_FLAGS}
)
