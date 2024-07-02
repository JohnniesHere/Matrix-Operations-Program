cmake_minimum_required(VERSION 3.28)
project(untitled10 C)

set(CMAKE_C_STANDARD 17)

add_executable(untitled10 MatricesCalculator.c
        testing.c)
# Link the math library
target_link_libraries(untitled10 m)
