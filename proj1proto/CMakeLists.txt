cmake_minimum_required(VERSION 3.15)
project(skipList C)

set(CMAKE_C_STANDARD 11)

add_executable(skipList main.c skipList.h)
add_executable(skipListTest ../proj1tests/skipListTests.c)