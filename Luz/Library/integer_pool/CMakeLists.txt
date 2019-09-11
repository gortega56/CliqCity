cmake_minimum_required(VERSION 3.15.2)

project(integer_pool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/integer_pool.h
			${CMAKE_CURRENT_SOURCE_DIR}/include/integer_coder.h)

add_library(integer_pool INTERFACE)

target_sources(integer_pool INTERFACE "$<BUILD_INTERFACE:${HEADERS}>")
target_include_directories(integer_pool INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(integer_pool SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

add_custom_target(integer-pool SOURCES ${HEADERS})
