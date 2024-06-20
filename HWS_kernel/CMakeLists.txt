cmake_minimum_required(VERSION 3.22)

set(SOURCE_FILES config_parser.cpp main.cpp)
add_executable(hws_kernel ${SOURCE_FILES})

target_link_libraries(hws_kernel sharedmemory)
