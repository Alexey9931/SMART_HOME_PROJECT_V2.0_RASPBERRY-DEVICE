project(hws_kernel)

set(SOURCE_FILES config_parser.cpp main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/loglib/>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/mysqldblib/>)

target_link_libraries(hws_kernel sharedmemory mysql_db device)
