#include "cpu_opcodes.h"
#include "mmu.h"

extern cpu_t g_cpu;

/*************** Helper funcions. ***************/

/**
 * Increment register n.
 * n = A,B,C,D,E,H,L,(HL)
 * Flags affected:
 * Z - Set if result is zero.
 * N - Reset.
 * H - Set if carry from bit 3.
 * C - Not affected.
 */
inline uint8_t inc_n(uint8_t value)
{
    if ((value & 0x0f) == 0x0f) {
        FLAG_SET(FLAG_H);
    } else {
        FLAG_CLEAR(FLAG_H);
    }
    ++value;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N);
    return value;
}

/*************** Opcodes implementaion. ***************/

/* 0x00 */
void nop()
{
}

/* 0x01: Load 16-bit immediate into BC */
void ld_bc_nn(uint16_t value)
{
    g_cpu.reg.bc = value;
}

/* 0x02: Save A to address pointed by BC */
void ld_xbc_a(void)
{
    mmu_write_byte(g_cpu.reg.bc, g_cpu.reg.a);
}

/* 0x03: Increment 16-bit BC. */
void inc_bc(void)
{
    g_cpu.reg.bc++;
}

/* 0x04: Increment B. */
void inc_b(void)
{
    g_cpu.reg.b = inc_n(g_cpu.reg.b);
}

/* 0x0C: Increment C. */
void inc_c(void)
{
    g_cpu.reg.c = inc_n(g_cpu.reg.c);
}

/* 0x13: Increment 16-bit DE. */
void inc_de(void)
{
    g_cpu.reg.de++;
}

/* 0x14: Increment D. */
void inc_d(void)
{
    g_cpu.reg.d = inc_n(g_cpu.reg.d);
}

/* 0x1C: Increment E. */
void inc_e(void)
{
    g_cpu.reg.e = inc_n(g_cpu.reg.e);
}

/* 0x23: Increment 16-bit HL. */
void inc_hl(void)
{
    g_cpu.reg.hl++;
}

/* 0x24: Increment H. */
void inc_h(void)
{
    g_cpu.reg.h = inc_n(g_cpu.reg.h);
}

/* 0x2C: Increment L. */
void inc_l(void)
{
    g_cpu.reg.l = inc_n(g_cpu.reg.l);
}

/* 0x33: Increment 16-bit SP. */
void inc_sp(void)
{
    g_cpu.reg.sp++;
}

/* 0x34: Increment value pointed by HL. */
void inc_xhl(void)
{
    uint8_t val = mmu_read_byte(g_cpu.reg.hl);
    mmu_write_byte(g_cpu.reg.hl, inc_n(val));
}

/* 0x3C: Increment A. */
void inc_a(void)
{
    g_cpu.reg.a = inc_n(g_cpu.reg.a);
}

