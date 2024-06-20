cmake_minimum_required(VERSION 3.22)

project(loglib)

add_library(log INTERFACE)
target_include_directories(log INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)
