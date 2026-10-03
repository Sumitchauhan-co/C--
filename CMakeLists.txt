cmake_minimum_required(VERSION 3.20)

project(c-- VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(c--
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
    src/runtime/environment.cpp
)

target_include_directories(c-- PRIVATE include)

if (MSVC)
    target_compile_options(c-- PRIVATE /W4)
else()
    target_compile_options(c-- PRIVATE -Wall -Wextra -Wpedantic)
endif()
