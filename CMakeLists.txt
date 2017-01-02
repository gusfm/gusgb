cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME gusgb)
project(${PROJECT_NAME} C CXX)

find_package(Threads REQUIRED)

SET (WARNINGS "-Werror -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=c11 -g ${WARNINGS}")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O2")
#set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=c11 -g")
#set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=c11 -pg")
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(SYSTEM "/usr/include/freetype2")

#-------------------
# Module source
#-------------------

add_library(libgusgb SHARED
    src/interrupt.c
    src/timer.c
    src/rom.c
    src/gpu.c
    src/mmu.c
    src/cpu_utils.c
    src/cpu_opcodes.c
    src/cpu_ext_ops.c
    src/cpu.c
    )
target_link_libraries(libgusgb m)

add_executable(${PROJECT_NAME}
    src/game_boy.c
    src/main.c)
target_link_libraries(${PROJECT_NAME} libgusgb GL glfw ftgl)
