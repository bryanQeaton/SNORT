cmake_minimum_required(VERSION 3.29)
project(SNORT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-DNDEBUG -Wall -Wno-dev")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(SNORT main.cpp
        snort.h)
