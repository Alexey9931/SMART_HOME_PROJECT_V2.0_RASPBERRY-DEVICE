cmake_minimum_required(VERSION 3.22)

project(sharedmemorylib)

set(SOURCE_FILES sh_memory.cpp)
set(HEADER_FILES sh_memory.hpp)

add_library(sharedmemory SHARED ${SOURCE_FILES} 
                                ${HEADER_FILES})
target_link_libraries(sharedmemory device log)
target_include_directories(sharedmemory PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
