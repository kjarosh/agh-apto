cmake_minimum_required(VERSION 3.14.3)
project(diaminy)
set(CMAKE_CXX_STANDARD 11)

include_directories(inc)

set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_target(onefile ALL)
add_custom_command(
    TARGET onefile
    COMMAND cat ${PROJECT_SOURCE_DIR}/inc/stdinc.h > onefile.cpp &&
        ${CMAKE_C_COMPILER} -E -DSKIP_STDINC ${PROJECT_SOURCE_DIR}/src/main.cpp -I${PROJECT_SOURCE_DIR}/inc -nostdinc >> onefile.cpp
)
