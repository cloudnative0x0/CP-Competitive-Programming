cmake_minimum_required(VERSION 3.16)
project(CP-Competitive-Programming CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)

add_library(pkg STATIC pkg/Profiler.cpp)
target_include_directories(pkg PUBLIC pkg)
target_link_libraries(pkg PUBLIC Threads::Threads)

set(EXCLUDED_DIRS pkg build .git .run-cache examples)

file(GLOB TOP_LEVEL_ENTRIES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/*)

foreach(entry ${TOP_LEVEL_ENTRIES})
    set(entry_path ${CMAKE_SOURCE_DIR}/${entry})
    if(IS_DIRECTORY ${entry_path} AND NOT entry IN_LIST EXCLUDED_DIRS)
        file(GLOB pattern_sources ${entry_path}/*.cpp)
        list(LENGTH pattern_sources n_sources)
        if(n_sources GREATER 0)
            add_executable(${entry} ${pattern_sources})
            target_link_libraries(${entry} PRIVATE pkg)
            message(STATUS "pattern target: ${entry}  (${pattern_sources})")
        endif()
    endif()
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/examples/main.cpp)
    add_executable(demo examples/main.cpp)
    target_link_libraries(demo PRIVATE pkg)
endif()
