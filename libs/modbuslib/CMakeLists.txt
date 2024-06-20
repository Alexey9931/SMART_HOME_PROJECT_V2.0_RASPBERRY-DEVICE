cmake_minimum_required(VERSION 3.22)

project(modbuslib)

set(SOURCE_FILES modbus.cpp)
set(HEADER_FILES modbus.hpp)

add_library(modbus SHARED ${SOURCE_FILES} 
                          ${HEADER_FILES})
target_include_directories(modbus PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
