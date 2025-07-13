project(mysql_db)

# sudo apt-get install curl libcurl4-openssl-dev
find_package(CURL REQUIRED)

set(SOURCE_FILES db_core.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/devicelib/>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/modbuslib/>)

# sudo apt-get install pkg-config
# sudo apt-get -y install libmysqlclient-dev
# sudo apt-get install mysql-server
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${LIBMYSQLCLIENT_LIBRARIES}
        ${CURL_LIBRARY}
        log)
