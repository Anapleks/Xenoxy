cmake_minimum_required(VERSION 3.20)

project(CMAKE_CXX_STDLIB_MODULES_JSON CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(XENOXY STATIC
    src/Xenoxy.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/EAST.cpp
    src/StateManager.cpp
    src/Interpreter.cpp
    src/Types.cpp

)

target_include_directories(XENOXY PUBLIC
    ${CMAKE_SOURCE_DIR}/include    
)

option(XENOXY_COMPILE_TESTS "Build Xenoxy tests" OFF)

if(XENOXY_COMPILE_TESTS)
    add_subdirectory(tests)
endif()
