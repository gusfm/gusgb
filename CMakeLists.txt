cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME gusgb)
project(${PROJECT_NAME} C CXX)

find_package(Threads REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

SET (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=gnu11 -O3 -fno-strict-aliasing ${WARNINGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O3 -fno-strict-aliasing")
include_directories(${PROJECT_SOURCE_DIR}/src)

# gusgb objects
add_library(gusgb_cart_obj OBJECT
    src/cartridge/mbc1.c
    src/cartridge/mbc3.c
    src/cartridge/cart.c
    )

add_library(gusgb_obj OBJECT
    src/interrupt.c
    src/timer.c
    src/gpu.c
    src/keys.c
    src/sound.c
    src/mmu.c
    src/cpu_utils.c
    src/cpu_opcodes.c
    src/cpu_ext_ops.c
    src/cpu.c
    src/game_boy.c
    )

# gusgb
add_executable(gusgb
    $<TARGET_OBJECTS:gusgb_cart_obj>
    $<TARGET_OBJECTS:gusgb_obj>
    src/main.c
    )
target_link_libraries(gusgb
    GL
    glfw
    )

# Objdump
add_executable(objdump
    src/objdump/objdump.c)

# gbas
BISON_TARGET(gbas_parser src/as/gbas.y ${CMAKE_CURRENT_BINARY_DIR}/gbas.tab.c)
FLEX_TARGET(gbas_scanner src/as/gbas.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(gbas_scanner gbas_parser)
add_executable(gbas
    ${BISON_gbas_parser_OUTPUTS}
    ${FLEX_gbas_scanner_OUTPUTS}
    src/as/opcodes.c
    src/as/gbas.c
    src/as/cart.c
    src/utils/list.c
    )
target_link_libraries(gbas fl m)

# gusgbai objects
add_library(gusgbai_obj OBJECT
    src/ai/random.c
    src/ai/neuron.c
    src/ai/neuron_layer.c
    src/ai/neural_net.c
    src/ai/player.c
    src/ai/genetic_alg.c
    src/ai/population.c
    )

# gusgbai
add_executable(gusgbai
    $<TARGET_OBJECTS:gusgb_cart_obj>
    $<TARGET_OBJECTS:gusgb_obj>
    $<TARGET_OBJECTS:gusgbai_obj>
    src/ai/gb_ai.c
    src/ai/main.c)
target_link_libraries(gusgbai
    GL
    glfw
    m
    )

enable_testing()

# gusgb test
add_executable(gusgbtest
    $<TARGET_OBJECTS:gusgb_cart_obj>
    test/cartridge/mbc3_test.cc
    )
target_link_libraries(gusgbtest
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
    )
add_test(test gusgbtest)

# gusgbai test
add_executable(gusgbaitest
    $<TARGET_OBJECTS:gusgbai_obj>
    test/ai/neuron_test.cc
    test/ai/neuron_layer_test.cc
    test/ai/neural_net_test.cc
    )
target_link_libraries(gusgbaitest
    gtest
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
    )
add_test(test gusgbaitest)
