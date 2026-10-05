cmake_minimum_required(VERSION 3.20)

project(cmm VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==============================================================================
# Global Compiler Settings
# ==============================================================================
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# Target 1: Main Pipeline Interpreter Executable (cmm)
# ==============================================================================
add_executable(cmm
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
    src/runtime/environment.cpp
)

target_include_directories(cmm PRIVATE include)

# ==============================================================================
# Target 2: Standalone Lexer Test Utility (lexer_demo)
# ==============================================================================
add_executable(lexer_demo 
    tests/lexer_demo.cpp 
    src/lexer/lexer.cpp
)

target_include_directories(lexer_demo PRIVATE include)
