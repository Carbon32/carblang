cmake_minimum_required(VERSION 3.15)
project(carblang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(CORE_SOURCES
    src/core/core.cpp
    src/core/error.cpp
    src/core/scanner.cpp
    src/core/token.cpp
    src/core/parser.cpp
)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(carblang src/carblang.cpp)
target_link_libraries(carblang PRIVATE core)

add_executable(test_core src/core/test_core.cpp)
target_link_libraries(test_core PRIVATE core)

add_test(NAME core_test COMMAND test_core)

set_target_properties(carblang test_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)