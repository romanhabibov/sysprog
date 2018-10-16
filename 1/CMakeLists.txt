cmake_minimum_required(VERSION 2.8)

project(1)

set(SOURCE_EXE main.cpp)

set(SOURCE_LIB functions.cpp)

find_package(Curses REQUIRED)

add_library(functions STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(main functions ${CURSES_LIBRARIES})
