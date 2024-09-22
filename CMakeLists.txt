cmake_minimum_required(VERSION 3.22)

project(smart_home_raspberry)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(libs)
add_subdirectory(HWS_kernel)
add_subdirectory(HWS_terminal)
