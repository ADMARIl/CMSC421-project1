cmake_minimum_required(VERSION 3.15)
project(skipList C)

set(CMAKE_C_STANDARD 11)

add_executable(skipList main.c skipList.h)
add_executable(skipListTest1 ../proj1tests/skipListTest1.c)
add_executable(skipListTest2 ../proj1tests/skipListTest2.c)