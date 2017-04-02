cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME gusgb)
project(${PROJECT_NAME} C CXX)

find_package(Threads REQUIRED)

SET (WARNINGS "-Werror -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes")

#set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=gnu11 -g")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")
set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=gnu11 -O3")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O3")
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(SYSTEM "/usr/include/freetype2")

#-------------------
# Module source
#-------------------

add_library(gusgb SHARED
    src/interrupt.c
    src/timer.c
    src/cart.c
    src/gpu.c
    src/keys.c
    src/mmu.c
    src/cpu_utils.c
    src/cpu_opcodes.c
    src/cpu_ext_ops.c
    src/cpu.c
    src/game_boy.c
    )
target_link_libraries(gusgb m)

add_executable(gusgb-bin
    src/main.c)
target_link_libraries(gusgb-bin gusgb GL glfw)
set_target_properties(gusgb-bin
    PROPERTIES
        COMPILE_FLAGS ${WARNINGS}
        OUTPUT_NAME gusgb
        )

# Objdump
add_executable(objdump
    src/objdump/objdump.c)

# gbas
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
BISON_TARGET(gbas_parser src/as/gbas.y ${CMAKE_CURRENT_BINARY_DIR}/gbas.tab.c)
FLEX_TARGET(gbas_scanner src/as/gbas.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
ADD_FLEX_BISON_DEPENDENCY(gbas_scanner gbas_parser)
add_executable(gbas
    ${BISON_gbas_parser_OUTPUTS}
    ${FLEX_gbas_scanner_OUTPUTS}
    src/as/opcodes.c
    src/as/gbas.c
    src/utils/list.c
    src/cart.c
    )
target_link_libraries(gbas fl m)

# libgusgbai
add_library(gusgbai SHARED
    src/ai/random.c
    src/ai/neuron.c
    src/ai/neuron_layer.c
    src/ai/neural_net.c
    src/ai/player.c
    src/ai/genetic_alg.c
    src/ai/population.c
    )
target_link_libraries(gusgbai m)
set_target_properties(gusgbai
    PROPERTIES
        COMPILE_FLAGS ${WARNINGS}
        )

# gusgbai
add_executable(gusgbai-bin
    src/ai/gb_ai.c
    src/ai/main.c)
target_link_libraries(gusgbai-bin
    gusgbai
    gusgb
    GL
    glfw
    )
set_target_properties(gusgbai-bin
    PROPERTIES
        COMPILE_FLAGS ${WARNINGS}
        OUTPUT_NAME gusgbai
        )

# gusgbai-training
add_executable(gusgbai-training
    src/ai/training.c
    )
target_link_libraries(gusgbai-training
    fann
    m
    )
set_target_properties(gusgbai-training
    PROPERTIES
        COMPILE_FLAGS ${WARNINGS}
        )

enable_testing()
# gusgbai test
add_executable(gusgbaitest
    test/ai/neuron_test.cc
    test/ai/neuron_layer_test.cc
    test/ai/neural_net_test.cc
    )
target_link_libraries(gusgbaitest
    gusgbai
    /usr/lib/libgtest.a
    /usr/lib/libgtest_main.a
    ${CMAKE_THREAD_LIBS_INIT}
    )
add_test(test gusgbaitest)
