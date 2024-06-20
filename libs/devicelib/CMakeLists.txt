cmake_minimum_required(VERSION 3.22)

project(devicelib)

set(SOURCE_FILES device_lib.cpp)
set(HEADER_FILES device_lib.hpp)

add_library(device SHARED ${SOURCE_FILES} 
                          ${HEADER_FILES})
target_link_libraries(device modbus)
target_include_directories(device PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
