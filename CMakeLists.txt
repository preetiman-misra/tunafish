cmake_minimum_required(VERSION 3.20)
project(tunafish VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(engine)
add_subdirectory(testbed)
