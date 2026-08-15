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

file(GLOB_RECURSE ALL_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/*.cpp)

set(TARGET_DIRS "")
foreach(src ${ALL_SOURCES})
    get_filename_component(src_dir ${src} DIRECTORY)

    if(NOT src_dir STREQUAL "")
        string(FIND ${src_dir} "/" first_slash)
        if(first_slash EQUAL -1)
            set(top_component ${src_dir})
        else()
            string(SUBSTRING ${src_dir} 0 ${first_slash} top_component)
        endif()

        if(NOT top_component IN_LIST EXCLUDED_DIRS)
            list(APPEND TARGET_DIRS ${src_dir})
        endif()
    endif()
endforeach()

list(REMOVE_DUPLICATES TARGET_DIRS)

foreach(dir ${TARGET_DIRS})
    file(GLOB dir_sources ${CMAKE_SOURCE_DIR}/${dir}/*.cpp)
    list(LENGTH dir_sources n_sources)
    if(n_sources GREATER 0)
        string(REPLACE "/" "_" target_name ${dir})
        add_executable(${target_name} ${dir_sources})
        target_link_libraries(${target_name} PRIVATE pkg)
        message(STATUS "pattern target: ${target_name}  (${dir_sources})")
    endif()
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/examples/main.cpp)
    add_executable(demo examples/main.cpp)
    target_link_libraries(demo PRIVATE pkg)
endif()
