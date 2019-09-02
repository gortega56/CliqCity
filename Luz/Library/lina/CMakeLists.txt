cmake_minimum_required(VERSION 3.15.2)

project(lina)

set(LINA_DIRECTORY ${LIBRARY_DIRECTORY}/lina/src)

set(HEADER_FILES ${LINA_DIRECTORY}/lina.h
				 ${LINA_DIRECTORY}/lina_types.h
				 ${LINA_DIRECTORY}/overlap.h)
				 
add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${HEADER_FILES}>")

target_include_directories(${PROJECT_NAME} INTERFACE "$<BUILD_INTERFACE:${LINA_DIRECTORY}>")


										 
