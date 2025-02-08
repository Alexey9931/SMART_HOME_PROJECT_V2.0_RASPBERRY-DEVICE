project(sharedmemory)

set(SOURCE_FILES sh_memory.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} log)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/devicelib/>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/libs/modbuslib/>)
