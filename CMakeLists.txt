cmake_minimum_required(VERSION 3.10)
project(carblang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

enable_testing()

set(SOURCES
    src/carblang.cpp
    src/core/core.cpp
)

add_executable(carblang ${SOURCES})

add_executable(test_core
    src/core/test_core.cpp
    src/core/core.cpp
)

target_include_directories(test_core PRIVATE include)
add_test(NAME core_test COMMAND test_core)
