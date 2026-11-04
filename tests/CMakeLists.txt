cmake_minimum_required(VERSION 3.20)

project(CMAKE_CXX_STDLIB_MODULES_JSON CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(de_test1
    directEast/test1.cpp
)

target_link_libraries(de_test1 PRIVATE
    XENOXY
)

add_executable(de_test2
    directEast/test2.cpp
)

target_link_libraries(de_test2 PRIVATE
    XENOXY
)

add_executable(de_test3
    directEast/test3.cpp
)

target_link_libraries(de_test3 PRIVATE
    XENOXY
)

add_executable(fp_test1
    fullPipeline/test1.cpp
)

target_link_libraries(fp_test1 PRIVATE
    XENOXY
)