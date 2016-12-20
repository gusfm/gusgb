#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>

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

#define FLAG_IS_ZERO (g_cpu.reg.f & FLAG_Z)
#define FLAG_IS_NEGATIVE (g_cpu.reg.f & FLAG_N)
#define FLAG_IS_CARRY (g_cpu.reg.f & FLAG_C)
#define FLAG_IS_HALFCARRY (g_cpu.reg.f & FLAG_H)
#define FLAG_SET(x) (g_cpu.reg.f |= (x))
#define FLAG_CLEAR(x) (g_cpu.reg.f &= ~(x))

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
} registers_t;

typedef struct {
    registers_t reg;
    uint32_t ticks;
} cpu_t;

int cpu_init(const char *rom_path);
void cpu_emulate_cycle(void);

#endif /* __CPU_H__ */
