cmake_minimum_required(VERSION 3.2)
project(ls_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(SOURCE_FILES main.c)
add_executable(ls ${SOURCE_FILES})
