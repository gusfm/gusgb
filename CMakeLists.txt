cmake_minimum_required(VERSION 2.8)
project(gusgb C)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

SET (WARNINGS "-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wstrict-prototypes")

set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -std=gnu11 -O2 -fno-strict-aliasing ${WARNINGS}")
include_directories(${PROJECT_SOURCE_DIR}/src)

# gusgb objects
add_library(gusgb_cart_obj OBJECT
    src/cartridge/mbc1.c
    src/cartridge/mbc3.c
    src/cartridge/mbc5.c
    src/cartridge/cart.c
    )

add_library(gusgb_obj OBJECT
    src/clock.c
    src/interrupt.c
    src/timer.c
    src/gpu.c
    src/keys.c
    src/apu/timer.c
    src/apu/length_counter.c
    src/apu/volume_envelope.c
    src/apu/sqr_ch.c
    src/apu/wave_ch.c
    src/apu/noise_ch.c
    src/apu/apu.c
    src/mmu.c
    src/cpu_utils.c
    src/cpu_opcodes.c
    src/cpu_ext_ops.c
    src/cpu.c
    src/gusgb.c
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
add_library(gbas_objs OBJECT
    src/utils/list.c
    src/as/str.c
    src/as/token.c
    src/as/lex.c
    src/as/op_enc.c
    src/as/parser.c
)

add_executable(gbas
    $<TARGET_OBJECTS:gbas_objs>
    src/as/cart.c
    src/as/main.c
)

enable_testing()

include_directories(${PROJECT_SOURCE_DIR}/test)

add_executable(cart_test
    $<TARGET_OBJECTS:gusgb_cart_obj>
    test/cartridge/mbc3.c
    test/cartridge/main.c
    )
add_test(test cart_test)

add_executable(cpu_test
    $<TARGET_OBJECTS:gbas_objs>
    src/clock.c
    src/interrupt.c
    src/timer.c
    src/cpu_utils.c
    src/cpu_opcodes.c
    src/cpu_ext_ops.c
    src/cpu.c
    test/cpu/mmu_mock.c
    test/cpu/asm.c
    test/cpu/parser_test.c
    test/cpu/cpu_test.c
    test/cpu/main.c
    )
add_test(test cart_test)
