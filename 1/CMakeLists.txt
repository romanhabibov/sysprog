cmake_minimum_required(VERSION 2.8)

project(1)

set(SOURCE_EXE main.c)

set(SOURCE_LIB functions.c)

add_library(functions STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main functions)
