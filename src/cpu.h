#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gpu.h"

/**
 * References:
 *
 * Writing a Game Boy emulator, Cinoop: https://cturt.github.io/cinoop.html
 * Game Boy CPU Manual: http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf
 * Opcode map for the Game Boy-Z80: http://imrannazar.com/Gameboy-Z80-Opcode-Map
 * GameBoy Opcode Summary: http://gameboy.mongenel.com/dmg/opcodes.html
 */

/**
 * Zero Flag (Z): This bit is set when the result of a math operation is zero
 * or two values match when using the CP instruction.
 */
#define FLAG_Z (1 << 7)
/**
 * Subtract Flag (N): This bit is set if a subtraction was performed in the last
 * math instruction.
 */
#define FLAG_N (1 << 6)
/**
 * Half Carry Flag (H): This bit is set if a carry occurred from the lower
 * nibble in the last math operation.
 */
#define FLAG_H (1 << 5)
/**
 * Carry Flag (C): This bit is set if a carry occurred from the last math
 * operation or if register A is the smaller value when executing the CP
 * instruction.
 */
#define FLAG_C (1 << 4)

#define FLAG_ANY (FLAG_C | FLAG_H | FLAG_N | FLAG_Z)

#define FLAG_IS_SET(flag) (uint8_t)(CPU.reg.f & flag)
#define FLAG_SET(x) (CPU.reg.f |= (x))
#define FLAG_CLEAR(x) (CPU.reg.f &= (uint8_t)(~(x)))
#define FLAG_SET_ZERO(value) \
    (CPU.reg.f = (uint8_t)((CPU.reg.f & 0x7f) | (((value)&1) << 7)))
#define FLAG_SET_CARRY(value) \
    (CPU.reg.f = (uint8_t)((CPU.reg.f & 0xef) | (((value)&1) << 4)))

/**
 * Z80 registers struct.
 */
typedef struct {
    struct {
        union {
            struct {
                uint8_t f;
                uint8_t a;
            };
            uint16_t af;
        };
    };

    struct {
        union {
            struct {
                uint8_t c;
                uint8_t b;
            };
            uint16_t bc;
        };
    };

    struct {
        union {
            struct {
                uint8_t e;
                uint8_t d;
            };
            uint16_t de;
        };
    };

    struct {
        union {
            struct {
                uint8_t l;
                uint8_t h;
            };
            uint16_t hl;
        };
    };

    uint16_t sp;
    uint16_t pc;
} cpu_registers_t;

typedef struct {
    cpu_registers_t reg;
    uint16_t last_pc;
    bool halt;
} cpu_t;

int cpu_init(const char *rom_path);
void cpu_finish(void);
void cpu_reset(void);
void cpu_emulate_cycle(void);
void cpu_dump(void);

#endif /* CPU_H */
