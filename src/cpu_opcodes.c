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
uint8_t inc_n(uint8_t value)
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

/**
 * Decrement register n.
 * n = A,B,C,D,E,H,L,(HL)
 * Flags affected:
 * Z - Set if result is zero.
 * N - Set.
 * H - Set if no borrow from bit 4.
 * C - Not affected.
 */
uint8_t dec_n(uint8_t value)
{
    if (value & 0x0f) {
        FLAG_CLEAR(FLAG_H);
    } else {
        FLAG_SET(FLAG_H);
    }
    ++value;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_SET(FLAG_N);
    return value;
}

/**
 * Add n to nn.
 * Flags affected:
 * Z - Not affected.
 * N - Reset.
 * H - Set if carry from bit 11.
 * C - Set if carry from bit 15.
 */
uint16_t add_nn_n(uint16_t val1, uint16_t val2)
{
    uint32_t result = val1 + val2;
    FLAG_CLEAR(FLAG_N);
    if (((val1 & 0x0f) + (val2 & 0x0f)) > 0x0f) {
        FLAG_SET(FLAG_H);
    } else {
        FLAG_CLEAR(FLAG_H);
    }
    if (result & 0xffff0000) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    return (uint16_t)(result & 0xffff);
}

/*************** Opcodes implementaion. ***************/

/* 0x00: No operation. */
void nop()
{
}

/* 0x01: Load 16-bit immediate into BC. */
void ld_bc_nn(uint16_t value)
{
    g_cpu.reg.bc = value;
}

/* 0x02: Save A to address pointed by BC. */
void ld_bcp_a(void)
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

/* 0x05: Decrement B. */
void dec_b(void)
{
    g_cpu.reg.b = dec_n(g_cpu.reg.b);
}

/* 0x06: Load 8-bit immediate into B. */
void ld_b_n(uint8_t val)
{
    g_cpu.reg.b = val;
}

/* 0x07: Rotate A left. Old bit 7 to Carry flag. */
void rlca(void)
{
    uint8_t carry = (g_cpu.reg.a & 0x80) >> 7;
    if (carry) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    g_cpu.reg.a <<= 1;
    g_cpu.reg.a += carry;
    FLAG_CLEAR(FLAG_N | FLAG_Z | FLAG_H);
}

/* 0x08: Save SP to given address. */
void ld_nnp_sp(uint16_t addr)
{
    mmu_write_word(addr, g_cpu.reg.sp);
}

/* 0x09: Add 16-bit BC to HL. */
void add_hl_bc(void)
{
    g_cpu.reg.hl = add_nn_n(g_cpu.reg.hl, g_cpu.reg.bc);
}

/* 0x0c: Increment C. */
void inc_c(void)
{
    g_cpu.reg.c = inc_n(g_cpu.reg.c);
}

/* 0x0d: Decrement C. */
void dec_c(void)
{
    g_cpu.reg.c = dec_n(g_cpu.reg.c);
}

/* 0x0e: Load 8-bit immediate into C. */
void ld_c_n(uint8_t val)
{
    g_cpu.reg.c = val;
}

/* 0x11: Load 16-bit immediate into DE. */
void ld_de_nn(uint16_t value)
{
    g_cpu.reg.de = value;
}

/* 0x12: Save A to address pointed by DE. */
void ld_dep_a(void)
{
    mmu_write_byte(g_cpu.reg.de, g_cpu.reg.a);
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

/* 0x15: Decrement D. */
void dec_d(void)
{
    g_cpu.reg.d = dec_n(g_cpu.reg.d);
}

/* 0x16: Load 8-bit immediate into D. */
void ld_d_n(uint8_t val)
{
    g_cpu.reg.d = val;
}

/* 0x19: Add 16-bit DE to HL. */
void add_hl_de(void)
{
    g_cpu.reg.hl = add_nn_n(g_cpu.reg.hl, g_cpu.reg.de);
}

/* 0x1c: Increment E. */
void inc_e(void)
{
    g_cpu.reg.e = inc_n(g_cpu.reg.e);
}

/* 0x1d: Decrement E. */
void dec_e(void)
{
    g_cpu.reg.e = dec_n(g_cpu.reg.e);
}

/* 0x1e: Load 8-bit immediate into E. */
void ld_e_n(uint8_t val)
{
    g_cpu.reg.e = val;
}

/* 0x21: Load 16-bit immediate into HL */
void ld_hl_nn(uint16_t value)
{
    g_cpu.reg.hl = value;
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

/* 0x25: Decrement H. */
void dec_h(void)
{
    g_cpu.reg.h = dec_n(g_cpu.reg.h);
}

/* 0x26: Load 8-bit immediate into H. */
void ld_h_n(uint8_t val)
{
    g_cpu.reg.h = val;
}

/* 0x29: Add 16-bit HL to HL. */
void add_hl_hl(void)
{
    g_cpu.reg.hl = add_nn_n(g_cpu.reg.hl, g_cpu.reg.hl);
}

/* 0x2c: Increment L. */
void inc_l(void)
{
    g_cpu.reg.l = inc_n(g_cpu.reg.l);
}

/* 0x2d: Decrement L. */
void dec_l(void)
{
    g_cpu.reg.l = dec_n(g_cpu.reg.l);
}

/* 0x2e: Load 8-bit immediate into L. */
void ld_l_n(uint8_t val)
{
    g_cpu.reg.l = val;
}

/* 0x31: Load 16-bit immediate into SP */
void ld_sp_nn(uint16_t value)
{
    g_cpu.reg.sp = value;
}

/* 0x33: Increment 16-bit SP. */
void inc_sp(void)
{
    g_cpu.reg.sp++;
}

/* 0x34: Increment value pointed by HL. */
void inc_hlp(void)
{
    uint8_t val = mmu_read_byte(g_cpu.reg.hl);
    mmu_write_byte(g_cpu.reg.hl, inc_n(val));
}

/* 0x35: Increment value pointed by HL. */
void dec_hlp(void)
{
    uint8_t val = mmu_read_byte(g_cpu.reg.hl);
    mmu_write_byte(g_cpu.reg.hl, dec_n(val));
}

/* 0x39: Add 16-bit SP to HL. */
void add_hl_sp(void)
{
    g_cpu.reg.hl = add_nn_n(g_cpu.reg.hl, g_cpu.reg.sp);
}

/* 0x3c: Increment A. */
void inc_a(void)
{
    g_cpu.reg.a = inc_n(g_cpu.reg.a);
}

/* 0x3d: Decrement A. */
void dec_a(void)
{
    g_cpu.reg.a = dec_n(g_cpu.reg.a);
}

/* 0x47: Copy A to B. */
void ld_b_a(void)
{
    g_cpu.reg.b = g_cpu.reg.a;
}

/* 0x4f: Copy A to C. */
void ld_c_a(void)
{
    g_cpu.reg.c = g_cpu.reg.a;
}

/* 0x57: Copy A to D. */
void ld_d_a(void)
{
    g_cpu.reg.d = g_cpu.reg.a;
}

/* 0x5f: Copy A to E. */
void ld_e_a(void)
{
    g_cpu.reg.e = g_cpu.reg.a;
}

/* 0x67: Copy A to H. */
void ld_h_a(void)
{
    g_cpu.reg.h = g_cpu.reg.a;
}

/* 0x6f: Copy A to L. */
void ld_l_a(void)
{
    g_cpu.reg.l = g_cpu.reg.a;
}

/* 0x77: Save A to address pointed by HL. */
void ld_hlp_a(void)
{
    mmu_write_byte(g_cpu.reg.hl, g_cpu.reg.a);
}

/* 0xea: Save A at given 16-bit address. */
void ld_nnp_a(uint16_t addr)
{
    mmu_write_byte(addr, g_cpu.reg.a);
}

