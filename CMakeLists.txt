cmake_minimum_required(VERSION 3.10)
project(carblang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/carblang.cpp
    src/core/core.cpp
)

add_executable(carblang ${SOURCES})
