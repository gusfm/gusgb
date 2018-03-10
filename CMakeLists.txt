cmake_minimum_required(VERSION 2.8)
project(gusgb C)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

SET (WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=gnu11 -O3 -fno-strict-aliasing ${WARNINGS}")
include_directories(${PROJECT_SOURCE_DIR}/src)

# gusgb objects
add_library(gusgb_cart_obj OBJECT
    src/cartridge/mbc1.c
    src/cartridge/mbc3.c
    src/cartridge/cart.c
    )

add_library(gusgb_obj OBJECT
    src/clock.c
    src/interrupt.c
    src/timer.c
    src/gpu.c
    src/keys.c
    src/apu.c
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
    ${SDL2_LIBRARIES}
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

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/test)

# gusgb test
add_executable(gusgbtest
    $<TARGET_OBJECTS:gusgb_cart_obj>
    test/cartridge/mbc3.c
    test/main.c
    )
add_test(test gusgbtest)
