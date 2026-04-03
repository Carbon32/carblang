cmake_minimum_required(VERSION 3.15)
project(carblang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    src/core/core.cpp
    src/core/error.cpp
    src/core/scanner.cpp
    src/core/token.cpp
    src/core/vm.cpp
    src/core/parser.cpp
    src/core/compiler.cpp
    src/core/module.cpp
    src/html/page.cpp
    src/html/tag.cpp 
    src/html/components.cpp
)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/src/html)

add_executable(carblang src/carblang.cpp)
target_link_libraries(carblang PRIVATE core)

set_target_properties(carblang PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)