cmake_minimum_required(VERSION 3.22)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
include(FindCurses)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(SOURCE_FILES graphic.cpp main.cpp)
add_executable(hws_terminal ${SOURCE_FILES})

target_link_libraries(hws_terminal sharedmemory 
                                   ${CURSES_LIBRARIES})
