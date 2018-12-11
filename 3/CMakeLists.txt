cmake_minimum_required (VERSION 2.6)

project (3)

set(SOURCE_EXE main.c)

set(SOURCE_LIB thread_pool.c)

find_package (Threads)

add_executable(main ${SOURCE_EXE})

add_library(thread_pool.c STATIC ${SOURCE_LIB})

target_link_libraries (main ${CMAKE_THREAD_LIBS_INIT} ${SOURCE_LIB})