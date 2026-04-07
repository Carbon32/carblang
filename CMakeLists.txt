cmake_minimum_required(VERSION 3.15)
project(carblang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

set(RAYLIB_PATH "C:/raylib")

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

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/html
)

add_executable(carblang src/carblang.cpp)

target_include_directories(carblang PRIVATE
    ${RAYLIB_PATH}/include
)

target_include_directories(core PUBLIC ${RAYLIB_PATH}/include)

target_link_directories(carblang PRIVATE
    ${RAYLIB_PATH}/lib
)

target_link_libraries(carblang PRIVATE
    core
    raylib
    opengl32
    gdi32
    winmm
)

set_target_properties(carblang PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)