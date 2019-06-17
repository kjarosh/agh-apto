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

add_custom_target(profile_mem)
add_custom_command(
    TARGET profile_mem
    COMMAND valgrind --tool=massif --massif-out-file=./massif.out
        $<TARGET_FILE:diaminy>
        < ${PROJECT_SOURCE_DIR}/../examples/7 &&
        massif-visualizer ./massif.out
    DEPENDS $<TARGET_FILE:diaminy>
)

add_custom_target(profile_cpu)
add_custom_command(
    TARGET profile_cpu
    COMMAND valgrind --tool=callgrind --callgrind-out-file=./callgrind.out
        $<TARGET_FILE:diaminy>
        < ${PROJECT_SOURCE_DIR}/../examples/7 &&
        kcachegrind ./callgrind.out
    DEPENDS $<TARGET_FILE:diaminy>
)
