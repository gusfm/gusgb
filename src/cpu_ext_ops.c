#include "cpu_ext_ops.h"
#include <stdio.h>
#include "clock.h"
#include "cpu.h"
#include "cpu_utils.h"
#include "mmu.h"

extern cpu_t CPU;

/* 0x00: Rotate B with carry. */
static void rlc_b(void)
{
    CPU.reg.b = rlc(CPU.reg.b);
}

/* 0x01: Rotate C with carry. */
static void rlc_c(void)
{
    CPU.reg.c = rlc(CPU.reg.c);
}

/* 0x02: Rotate D with carry. */
static void rlc_d(void)
{
    CPU.reg.d = rlc(CPU.reg.d);
}

/* 0x03: Rotate E with carry. */
static void rlc_e(void)
{
    CPU.reg.e = rlc(CPU.reg.e);
}

/* 0x04: Rotate H with carry. */
static void rlc_h(void)
{
    CPU.reg.h = rlc(CPU.reg.h);
}

/* 0x05: Rotate L with carry. */
static void rlc_l(void)
{
    CPU.reg.l = rlc(CPU.reg.l);
}

/* 0x06: Rotate (HL) with carry. */
static void rlc_hlp(void)
{
    uint8_t val = rlc(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x07: Rotate A with carry. */
static void rlc_a(void)
{
    CPU.reg.a = rlc(CPU.reg.a);
}

/* 0x08: Rotate B with carry. */
static void rrc_b(void)
{
    CPU.reg.b = rrc(CPU.reg.b);
}

/* 0x09: Rotate C with carry. */
static void rrc_c(void)
{
    CPU.reg.c = rrc(CPU.reg.c);
}

/* 0x0a: Rotate D with carry. */
static void rrc_d(void)
{
    CPU.reg.d = rrc(CPU.reg.d);
}

/* 0x0b: Rotate E with carry. */
static void rrc_e(void)
{
    CPU.reg.e = rrc(CPU.reg.e);
}

/* 0x0c: Rotate H with carry. */
static void rrc_h(void)
{
    CPU.reg.h = rrc(CPU.reg.h);
}

/* 0x0d: Rotate L with carry. */
static void rrc_l(void)
{
    CPU.reg.l = rrc(CPU.reg.l);
}

/* 0x0e: Rotate (HL) with carry. */
static void rrc_hlp(void)
{
    uint8_t val = rrc(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x0f: Rotate A with carry. */
static void rrc_a(void)
{
    CPU.reg.a = rrc(CPU.reg.a);
}

/* 0x10: Rotate B left through Carry flag. */
static void rl_b(void)
{
    CPU.reg.b = rl(CPU.reg.b);
}

/* 0x11: Rotate C left through Carry flag. */
static void rl_c(void)
{
    CPU.reg.c = rl(CPU.reg.c);
}

/* 0x12: Rotate D left through Carry flag. */
static void rl_d(void)
{
    CPU.reg.d = rl(CPU.reg.d);
}

/* 0x13: Rotate E left through Carry flag. */
static void rl_e(void)
{
    CPU.reg.e = rl(CPU.reg.e);
}

/* 0x14: Rotate H left through Carry flag. */
static void rl_h(void)
{
    CPU.reg.h = rl(CPU.reg.h);
}

/* 0x15: Rotate L left through Carry flag. */
static void rl_l(void)
{
    CPU.reg.l = rl(CPU.reg.l);
}

/* 0x16: Rotate (HL) with carry. */
static void rl_hlp(void)
{
    uint8_t val = rl(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x17: Rotate A left through Carry flag. */
static void rl_a(void)
{
    CPU.reg.a = rl(CPU.reg.a);
}

/* 0x18: Rotate B right through carry flag. */
static void rr_b(void)
{
    CPU.reg.b = rr(CPU.reg.b);
}

/* 0x19: Rotate C right through carry flag. */
static void rr_c(void)
{
    CPU.reg.c = rr(CPU.reg.c);
}

/* 0x1a: Rotate D right through carry flag. */
static void rr_d(void)
{
    CPU.reg.d = rr(CPU.reg.d);
}

/* 0x1b: Rotate E right through carry flag. */
static void rr_e(void)
{
    CPU.reg.e = rr(CPU.reg.e);
}

/* 0x1c: Rotate H right through carry flag. */
static void rr_h(void)
{
    CPU.reg.h = rr(CPU.reg.h);
}

/* 0x1d: Rotate L right through carry flag. */
static void rr_l(void)
{
    CPU.reg.l = rr(CPU.reg.l);
}

/* 0x1e: Rotate (HL) right through carry flag. */
static void rr_hlp(void)
{
    uint8_t val = rr(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x1f: Rotate A right through carry flag. */
static void rr_a(void)
{
    CPU.reg.a = rr(CPU.reg.a);
}

/* 0x20: Shift B left into Carry flag. */
static void sla_b(void)
{
    CPU.reg.b = sla(CPU.reg.b);
}

/* 0x21: Shift C left into Carry flag. */
static void sla_c(void)
{
    CPU.reg.c = sla(CPU.reg.c);
}

/* 0x22: Shift D left into Carry flag. */
static void sla_d(void)
{
    CPU.reg.d = sla(CPU.reg.d);
}

/* 0x23: Shift E left into Carry flag. */
static void sla_e(void)
{
    CPU.reg.e = sla(CPU.reg.e);
}

/* 0x24: Shift H left into Carry flag. */
static void sla_h(void)
{
    CPU.reg.h = sla(CPU.reg.h);
}

/* 0x25: Shift L left into Carry flag. */
static void sla_l(void)
{
    CPU.reg.l = sla(CPU.reg.l);
}

/* 0x26: Shift (HL) with carry. */
static void sla_hlp(void)
{
    uint8_t val = sla(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x27: Shift A left into Carry flag. */
static void sla_a(void)
{
    CPU.reg.a = sla(CPU.reg.a);
}

/* 0x28: Shift B right into Carry flag. */
static void sra_b(void)
{
    CPU.reg.b = sra(CPU.reg.b);
}

/* 0x29: Shift C right into Carry flag. */
static void sra_c(void)
{
    CPU.reg.c = sra(CPU.reg.c);
}

/* 0x2a: Shift D right into Carry flag. */
static void sra_d(void)
{
    CPU.reg.d = sra(CPU.reg.d);
}

/* 0x2b: Shift E right into Carry flag. */
static void sra_e(void)
{
    CPU.reg.e = sra(CPU.reg.e);
}

/* 0x2c: Shift H right into Carry flag. */
static void sra_h(void)
{
    CPU.reg.h = sra(CPU.reg.h);
}

/* 0x2d: Shift L right into Carry flag. */
static void sra_l(void)
{
    CPU.reg.l = sra(CPU.reg.l);
}

/* 0x2e: Shift (HL) right into Carry flag. */
static void sra_hlp(void)
{
    uint8_t val = sra(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x2f: Shift A right into Carry flag. */
static void sra_a(void)
{
    CPU.reg.a = sra(CPU.reg.a);
}

/* 0x30: Swap upper & lower nibbles of n. */
static void swap_b(void)
{
    CPU.reg.b = swap(CPU.reg.b);
}

/* 0x31: Swap upper & lower nibbles of n. */
static void swap_c(void)
{
    CPU.reg.c = swap(CPU.reg.c);
}

/* 0x32: Swap upper & lower nibbles of n. */
static void swap_d(void)
{
    CPU.reg.d = swap(CPU.reg.d);
}

/* 0x33: Swap upper & lower nibbles of n. */
static void swap_e(void)
{
    CPU.reg.e = swap(CPU.reg.e);
}

/* 0x34: Swap upper & lower nibbles of n. */
static void swap_h(void)
{
    CPU.reg.h = swap(CPU.reg.h);
}

/* 0x35: Swap upper & lower nibbles of n. */
static void swap_l(void)
{
    CPU.reg.l = swap(CPU.reg.l);
}

/* 0x36: Swap upper & lower nibbles of n. */
static void swap_hlp(void)
{
    uint8_t val = swap(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x37: Swap upper & lower nibbles of n. */
static void swap_a(void)
{
    CPU.reg.a = swap(CPU.reg.a);
}

/* 0x38: Shift B right into Carry flag. */
static void srl_b(void)
{
    CPU.reg.b = srl(CPU.reg.b);
}

/* 0x39: Shift C right into Carry flag. */
static void srl_c(void)
{
    CPU.reg.c = srl(CPU.reg.c);
}

/* 0x3a: Shift D right into Carry flag. */
static void srl_d(void)
{
    CPU.reg.d = srl(CPU.reg.d);
}

/* 0x3b: Shift E right into Carry flag. */
static void srl_e(void)
{
    CPU.reg.e = srl(CPU.reg.e);
}

/* 0x3c: Shift H right into Carry flag. */
static void srl_h(void)
{
    CPU.reg.h = srl(CPU.reg.h);
}

/* 0x3d: Shift L right into Carry flag. */
static void srl_l(void)
{
    CPU.reg.l = srl(CPU.reg.l);
}

/* 0x3e: Shift (HL) right into Carry flag. */
static void srl_hlp(void)
{
    uint8_t val = srl(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x3f: Shift A right into Carry flag. */
static void srl_a(void)
{
    CPU.reg.a = srl(CPU.reg.a);
}

/* 0x40: Test bit in register. */
static void bit_0_b(void)
{
    bit(1 << 0, CPU.reg.b);
}

/* 0x41: Test bit in register. */
static void bit_0_c(void)
{
    bit(1 << 0, CPU.reg.c);
}

/* 0x42: Test bit in register. */
static void bit_0_d(void)
{
    bit(1 << 0, CPU.reg.d);
}

/* 0x43: Test bit in register. */
static void bit_0_e(void)
{
    bit(1 << 0, CPU.reg.e);
}

/* 0x44: Test bit in register. */
static void bit_0_h(void)
{
    bit(1 << 0, CPU.reg.h);
}

/* 0x45: Test bit in register. */
static void bit_0_l(void)
{
    bit(1 << 0, CPU.reg.l);
}

/* 0x46: Test bit in register. */
static void bit_0_hlp(void)
{
    bit(1 << 0, mmu_read_byte(CPU.reg.hl));
}

/* 0x47: Test bit in register. */
static void bit_0_a(void)
{
    bit(1 << 0, CPU.reg.a);
}

/* 0x48: Test bit in register. */
static void bit_1_b(void)
{
    bit(1 << 1, CPU.reg.b);
}

/* 0x49: Test bit in register. */
static void bit_1_c(void)
{
    bit(1 << 1, CPU.reg.c);
}

/* 0x4a: Test bit in register. */
static void bit_1_d(void)
{
    bit(1 << 1, CPU.reg.d);
}

/* 0x4b: Test bit in register. */
static void bit_1_e(void)
{
    bit(1 << 1, CPU.reg.e);
}

/* 0x4c: Test bit in register. */
static void bit_1_h(void)
{
    bit(1 << 1, CPU.reg.h);
}

/* 0x4d: Test bit in register. */
static void bit_1_l(void)
{
    bit(1 << 1, CPU.reg.l);
}

/* 0x4e: Test bit in register. */
static void bit_1_hlp(void)
{
    bit(1 << 1, mmu_read_byte(CPU.reg.hl));
}

/* 0x4f: Test bit in register. */
static void bit_1_a(void)
{
    bit(1 << 1, CPU.reg.a);
}

/* 0x50: Test bit in register. */
static void bit_2_b(void)
{
    bit(1 << 2, CPU.reg.b);
}

/* 0x51: Test bit in register. */
static void bit_2_c(void)
{
    bit(1 << 2, CPU.reg.c);
}

/* 0x52: Test bit in register. */
static void bit_2_d(void)
{
    bit(1 << 2, CPU.reg.d);
}

/* 0x53: Test bit in register. */
static void bit_2_e(void)
{
    bit(1 << 2, CPU.reg.e);
}

/* 0x54: Test bit in register. */
static void bit_2_h(void)
{
    bit(1 << 2, CPU.reg.h);
}

/* 0x55: Test bit in register. */
static void bit_2_l(void)
{
    bit(1 << 2, CPU.reg.l);
}

/* 0x56: Test bit in register. */
static void bit_2_hlp(void)
{
    bit(1 << 2, mmu_read_byte(CPU.reg.hl));
}

/* 0x57: Test bit in register. */
static void bit_2_a(void)
{
    bit(1 << 2, CPU.reg.a);
}

/* 0x58: Test bit in register. */
static void bit_3_b(void)
{
    bit(1 << 3, CPU.reg.b);
}

/* 0x59: Test bit in register. */
static void bit_3_c(void)
{
    bit(1 << 3, CPU.reg.c);
}

/* 0x5a: Test bit in register. */
static void bit_3_d(void)
{
    bit(1 << 3, CPU.reg.d);
}

/* 0x5b: Test bit in register. */
static void bit_3_e(void)
{
    bit(1 << 3, CPU.reg.e);
}

/* 0x5c: Test bit in register. */
static void bit_3_h(void)
{
    bit(1 << 3, CPU.reg.h);
}

/* 0x5d: Test bit in register. */
static void bit_3_l(void)
{
    bit(1 << 3, CPU.reg.l);
}

/* 0x5e: Test bit in register. */
static void bit_3_hlp(void)
{
    bit(1 << 3, mmu_read_byte(CPU.reg.hl));
}

/* 0x5f: Test bit in register. */
static void bit_3_a(void)
{
    bit(1 << 3, CPU.reg.a);
}

/* 0x60: Test bit in register. */
static void bit_4_b(void)
{
    bit(1 << 4, CPU.reg.b);
}

/* 0x61: Test bit in register. */
static void bit_4_c(void)
{
    bit(1 << 4, CPU.reg.c);
}

/* 0x62: Test bit in register. */
static void bit_4_d(void)
{
    bit(1 << 4, CPU.reg.d);
}

/* 0x63: Test bit in register. */
static void bit_4_e(void)
{
    bit(1 << 4, CPU.reg.e);
}

/* 0x64: Test bit in register. */
static void bit_4_h(void)
{
    bit(1 << 4, CPU.reg.h);
}

/* 0x65: Test bit in register. */
static void bit_4_l(void)
{
    bit(1 << 4, CPU.reg.l);
}

/* 0x66: Test bit in register. */
static void bit_4_hlp(void)
{
    bit(1 << 4, mmu_read_byte(CPU.reg.hl));
}

/* 0x67: Test bit in register. */
static void bit_4_a(void)
{
    bit(1 << 4, CPU.reg.a);
}

/* 0x68: Test bit in register. */
static void bit_5_b(void)
{
    bit(1 << 5, CPU.reg.b);
}

/* 0x69: Test bit in register. */
static void bit_5_c(void)
{
    bit(1 << 5, CPU.reg.c);
}

/* 0x6a: Test bit in register. */
static void bit_5_d(void)
{
    bit(1 << 5, CPU.reg.d);
}

/* 0x6b: Test bit in register. */
static void bit_5_e(void)
{
    bit(1 << 5, CPU.reg.e);
}

/* 0x6c: Test bit in register. */
static void bit_5_h(void)
{
    bit(1 << 5, CPU.reg.h);
}

/* 0x6d: Test bit in register. */
static void bit_5_l(void)
{
    bit(1 << 5, CPU.reg.l);
}

/* 0x6e: Test bit in register. */
static void bit_5_hlp(void)
{
    bit(1 << 5, mmu_read_byte(CPU.reg.hl));
}

/* 0x6f: Test bit in register. */
static void bit_5_a(void)
{
    bit(1 << 5, CPU.reg.a);
}

/* 0x70: Test bit in register. */
static void bit_6_b(void)
{
    bit(1 << 6, CPU.reg.b);
}

/* 0x71: Test bit in register. */
static void bit_6_c(void)
{
    bit(1 << 6, CPU.reg.c);
}

/* 0x72: Test bit in register. */
static void bit_6_d(void)
{
    bit(1 << 6, CPU.reg.d);
}

/* 0x73: Test bit in register. */
static void bit_6_e(void)
{
    bit(1 << 6, CPU.reg.e);
}

/* 0x74: Test bit in register. */
static void bit_6_h(void)
{
    bit(1 << 6, CPU.reg.h);
}

/* 0x75: Test bit in register. */
static void bit_6_l(void)
{
    bit(1 << 6, CPU.reg.l);
}

/* 0x76: Test bit in register. */
static void bit_6_hlp(void)
{
    bit(1 << 6, mmu_read_byte(CPU.reg.hl));
}

/* 0x77: Test bit in register. */
static void bit_6_a(void)
{
    bit(1 << 6, CPU.reg.a);
}

/* 0x78: Test bit in register. */
static void bit_7_b(void)
{
    bit(1 << 7, CPU.reg.b);
}

/* 0x79: Test bit in register. */
static void bit_7_c(void)
{
    bit(1 << 7, CPU.reg.c);
}

/* 0x7a: Test bit in register. */
static void bit_7_d(void)
{
    bit(1 << 7, CPU.reg.d);
}

/* 0x7b: Test bit in register. */
static void bit_7_e(void)
{
    bit(1 << 7, CPU.reg.e);
}

/* 0x7c: Test bit in register. */
static void bit_7_h(void)
{
    bit(1 << 7, CPU.reg.h);
}

/* 0x7d: Test bit in register. */
static void bit_7_l(void)
{
    bit(1 << 7, CPU.reg.l);
}

/* 0x7e: Test bit in register. */
static void bit_7_hlp(void)
{
    bit(1 << 7, mmu_read_byte(CPU.reg.hl));
}

/* 0x7f: Test bit in register. */
static void bit_7_a(void)
{
    bit(1 << 7, CPU.reg.a);
}

/* 0x80: Reset bit in register. */
static void res_0_b(void)
{
    CPU.reg.b = res(1 << 0, CPU.reg.b);
}

/* 0x81: Reset bit in register. */
static void res_0_c(void)
{
    CPU.reg.c = res(1 << 0, CPU.reg.c);
}

/* 0x82: Reset bit in register. */
static void res_0_d(void)
{
    CPU.reg.d = res(1 << 0, CPU.reg.d);
}

/* 0x83: Reset bit in register. */
static void res_0_e(void)
{
    CPU.reg.e = res(1 << 0, CPU.reg.e);
}

/* 0x84: Reset bit in register. */
static void res_0_h(void)
{
    CPU.reg.h = res(1 << 0, CPU.reg.h);
}

/* 0x85: Reset bit in register. */
static void res_0_l(void)
{
    CPU.reg.l = res(1 << 0, CPU.reg.l);
}

/* 0x86: Reset bit in register. */
static void res_0_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 0, mmu_read_byte(CPU.reg.hl)));
}

/* 0x87: Reset bit in register. */
static void res_0_a(void)
{
    CPU.reg.a = res(1 << 0, CPU.reg.a);
}

/* 0x88: Reset bit in register. */
static void res_1_b(void)
{
    CPU.reg.b = res(1 << 1, CPU.reg.b);
}

/* 0x89: Reset bit in register. */
static void res_1_c(void)
{
    CPU.reg.c = res(1 << 1, CPU.reg.c);
}

/* 0x8a: Reset bit in register. */
static void res_1_d(void)
{
    CPU.reg.d = res(1 << 1, CPU.reg.d);
}

/* 0x8b: Reset bit in register. */
static void res_1_e(void)
{
    CPU.reg.e = res(1 << 1, CPU.reg.e);
}

/* 0x8c: Reset bit in register. */
static void res_1_h(void)
{
    CPU.reg.h = res(1 << 1, CPU.reg.h);
}

/* 0x8d: Reset bit in register. */
static void res_1_l(void)
{
    CPU.reg.l = res(1 << 1, CPU.reg.l);
}

/* 0x8e: Reset bit in register. */
static void res_1_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 1, mmu_read_byte(CPU.reg.hl)));
}

/* 0x8f: Reset bit in register. */
static void res_1_a(void)
{
    CPU.reg.a = res(1 << 1, CPU.reg.a);
}

/* 0x90: Reset bit in register. */
static void res_2_b(void)
{
    CPU.reg.b = res(1 << 2, CPU.reg.b);
}

/* 0x91: Reset bit in register. */
static void res_2_c(void)
{
    CPU.reg.c = res(1 << 2, CPU.reg.c);
}

/* 0x92: Reset bit in register. */
static void res_2_d(void)
{
    CPU.reg.d = res(1 << 2, CPU.reg.d);
}

/* 0x93: Reset bit in register. */
static void res_2_e(void)
{
    CPU.reg.e = res(1 << 2, CPU.reg.e);
}

/* 0x94: Reset bit in register. */
static void res_2_h(void)
{
    CPU.reg.h = res(1 << 2, CPU.reg.h);
}

/* 0x95: Reset bit in register. */
static void res_2_l(void)
{
    CPU.reg.l = res(1 << 2, CPU.reg.l);
}

/* 0x96: Reset bit in register. */
static void res_2_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 2, mmu_read_byte(CPU.reg.hl)));
}

/* 0x97: Reset bit in register. */
static void res_2_a(void)
{
    CPU.reg.a = res(1 << 2, CPU.reg.a);
}

/* 0x98: Reset bit in register. */
static void res_3_b(void)
{
    CPU.reg.b = res(1 << 3, CPU.reg.b);
}

/* 0x99: Reset bit in register. */
static void res_3_c(void)
{
    CPU.reg.c = res(1 << 3, CPU.reg.c);
}

/* 0x9a: Reset bit in register. */
static void res_3_d(void)
{
    CPU.reg.d = res(1 << 3, CPU.reg.d);
}

/* 0x9b: Reset bit in register. */
static void res_3_e(void)
{
    CPU.reg.e = res(1 << 3, CPU.reg.e);
}

/* 0x9c: Reset bit in register. */
static void res_3_h(void)
{
    CPU.reg.h = res(1 << 3, CPU.reg.h);
}

/* 0x9d: Reset bit in register. */
static void res_3_l(void)
{
    CPU.reg.l = res(1 << 3, CPU.reg.l);
}

/* 0x9e: Reset bit in register. */
static void res_3_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 3, mmu_read_byte(CPU.reg.hl)));
}

/* 0x9f: Reset bit in register. */
static void res_3_a(void)
{
    CPU.reg.a = res(1 << 3, CPU.reg.a);
}

/* 0xa0: Reset bit in register. */
static void res_4_b(void)
{
    CPU.reg.b = res(1 << 4, CPU.reg.b);
}

/* 0xa1: Reset bit in register. */
static void res_4_c(void)
{
    CPU.reg.c = res(1 << 4, CPU.reg.c);
}

/* 0xa2: Reset bit in register. */
static void res_4_d(void)
{
    CPU.reg.d = res(1 << 4, CPU.reg.d);
}

/* 0xa3: Reset bit in register. */
static void res_4_e(void)
{
    CPU.reg.e = res(1 << 4, CPU.reg.e);
}

/* 0xa4: Reset bit in register. */
static void res_4_h(void)
{
    CPU.reg.h = res(1 << 4, CPU.reg.h);
}

/* 0xa5: Reset bit in register. */
static void res_4_l(void)
{
    CPU.reg.l = res(1 << 4, CPU.reg.l);
}

/* 0xa6: Reset bit in register. */
static void res_4_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 4, mmu_read_byte(CPU.reg.hl)));
}

/* 0xa7: Reset bit in register. */
static void res_4_a(void)
{
    CPU.reg.a = res(1 << 4, CPU.reg.a);
}

/* 0xa8: Reset bit in register. */
static void res_5_b(void)
{
    CPU.reg.b = res(1 << 5, CPU.reg.b);
}

/* 0xa9: Reset bit in register. */
static void res_5_c(void)
{
    CPU.reg.c = res(1 << 5, CPU.reg.c);
}

/* 0xaa: Reset bit in register. */
static void res_5_d(void)
{
    CPU.reg.d = res(1 << 5, CPU.reg.d);
}

/* 0xab: Reset bit in register. */
static void res_5_e(void)
{
    CPU.reg.e = res(1 << 5, CPU.reg.e);
}

/* 0xac: Reset bit in register. */
static void res_5_h(void)
{
    CPU.reg.h = res(1 << 5, CPU.reg.h);
}

/* 0xad: Reset bit in register. */
static void res_5_l(void)
{
    CPU.reg.l = res(1 << 5, CPU.reg.l);
}

/* 0xae: Reset bit in register. */
static void res_5_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 5, mmu_read_byte(CPU.reg.hl)));
}

/* 0xaf: Reset bit in register. */
static void res_5_a(void)
{
    CPU.reg.a = res(1 << 5, CPU.reg.a);
}

/* 0xb0: Reset bit in register. */
static void res_6_b(void)
{
    CPU.reg.b = res(1 << 6, CPU.reg.b);
}

/* 0xb1: Reset bit in register. */
static void res_6_c(void)
{
    CPU.reg.c = res(1 << 6, CPU.reg.c);
}

/* 0xb2: Reset bit in register. */
static void res_6_d(void)
{
    CPU.reg.d = res(1 << 6, CPU.reg.d);
}

/* 0xb3: Reset bit in register. */
static void res_6_e(void)
{
    CPU.reg.e = res(1 << 6, CPU.reg.e);
}

/* 0xb4: Reset bit in register. */
static void res_6_h(void)
{
    CPU.reg.h = res(1 << 6, CPU.reg.h);
}

/* 0xb5: Reset bit in register. */
static void res_6_l(void)
{
    CPU.reg.l = res(1 << 6, CPU.reg.l);
}

/* 0xb6: Reset bit in register. */
static void res_6_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 6, mmu_read_byte(CPU.reg.hl)));
}

/* 0xb7: Reset bit in register. */
static void res_6_a(void)
{
    CPU.reg.a = res(1 << 6, CPU.reg.a);
}

/* 0xb8: Reset bit in register. */
static void res_7_b(void)
{
    CPU.reg.b = res(1 << 7, CPU.reg.b);
}

/* 0xb9: Reset bit in register. */
static void res_7_c(void)
{
    CPU.reg.c = res(1 << 7, CPU.reg.c);
}

/* 0xba: Reset bit in register. */
static void res_7_d(void)
{
    CPU.reg.d = res(1 << 7, CPU.reg.d);
}

/* 0xbb: Reset bit in register. */
static void res_7_e(void)
{
    CPU.reg.e = res(1 << 7, CPU.reg.e);
}

/* 0xbc: Reset bit in register. */
static void res_7_h(void)
{
    CPU.reg.h = res(1 << 7, CPU.reg.h);
}

/* 0xbd: Reset bit in register. */
static void res_7_l(void)
{
    CPU.reg.l = res(1 << 7, CPU.reg.l);
}

/* 0xbe: Reset bit in register. */
static void res_7_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 7, mmu_read_byte(CPU.reg.hl)));
}

/* 0xbf: Reset bit in register. */
static void res_7_a(void)
{
    CPU.reg.a = res(1 << 7, CPU.reg.a);
}

/* 0xc0: Reset bit in register. */
static void set_0_b(void)
{
    CPU.reg.b = set(1 << 0, CPU.reg.b);
}

/* 0xc1: Reset bit in register. */
static void set_0_c(void)
{
    CPU.reg.c = set(1 << 0, CPU.reg.c);
}

/* 0xc2: Reset bit in register. */
static void set_0_d(void)
{
    CPU.reg.d = set(1 << 0, CPU.reg.d);
}

/* 0xc3: Reset bit in register. */
static void set_0_e(void)
{
    CPU.reg.e = set(1 << 0, CPU.reg.e);
}

/* 0xc4: Reset bit in register. */
static void set_0_h(void)
{
    CPU.reg.h = set(1 << 0, CPU.reg.h);
}

/* 0xc5: Reset bit in register. */
static void set_0_l(void)
{
    CPU.reg.l = set(1 << 0, CPU.reg.l);
}

/* 0xc6: Reset bit in register. */
static void set_0_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 0, mmu_read_byte(CPU.reg.hl)));
}

/* 0xc7: Reset bit in register. */
static void set_0_a(void)
{
    CPU.reg.a = set(1 << 0, CPU.reg.a);
}

/* 0xc8: Reset bit in register. */
static void set_1_b(void)
{
    CPU.reg.b = set(1 << 1, CPU.reg.b);
}

/* 0xc9: Reset bit in register. */
static void set_1_c(void)
{
    CPU.reg.c = set(1 << 1, CPU.reg.c);
}

/* 0xca: Reset bit in register. */
static void set_1_d(void)
{
    CPU.reg.d = set(1 << 1, CPU.reg.d);
}

/* 0xcb: Reset bit in register. */
static void set_1_e(void)
{
    CPU.reg.e = set(1 << 1, CPU.reg.e);
}

/* 0xcc: Reset bit in register. */
static void set_1_h(void)
{
    CPU.reg.h = set(1 << 1, CPU.reg.h);
}

/* 0xcd: Reset bit in register. */
static void set_1_l(void)
{
    CPU.reg.l = set(1 << 1, CPU.reg.l);
}

/* 0xce: Reset bit in register. */
static void set_1_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 1, mmu_read_byte(CPU.reg.hl)));
}

/* 0xcf: Reset bit in register. */
static void set_1_a(void)
{
    CPU.reg.a = set(1 << 1, CPU.reg.a);
}

/* 0xd0: Reset bit in register. */
static void set_2_b(void)
{
    CPU.reg.b = set(1 << 2, CPU.reg.b);
}

/* 0xd1: Reset bit in register. */
static void set_2_c(void)
{
    CPU.reg.c = set(1 << 2, CPU.reg.c);
}

/* 0xd2: Reset bit in register. */
static void set_2_d(void)
{
    CPU.reg.d = set(1 << 2, CPU.reg.d);
}

/* 0xd3: Reset bit in register. */
static void set_2_e(void)
{
    CPU.reg.e = set(1 << 2, CPU.reg.e);
}

/* 0xd4: Reset bit in register. */
static void set_2_h(void)
{
    CPU.reg.h = set(1 << 2, CPU.reg.h);
}

/* 0xd5: Reset bit in register. */
static void set_2_l(void)
{
    CPU.reg.l = set(1 << 2, CPU.reg.l);
}

/* 0xd6: Reset bit in register. */
static void set_2_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 2, mmu_read_byte(CPU.reg.hl)));
}

/* 0xd7: Reset bit in register. */
static void set_2_a(void)
{
    CPU.reg.a = set(1 << 2, CPU.reg.a);
}

/* 0xd8: Reset bit in register. */
static void set_3_b(void)
{
    CPU.reg.b = set(1 << 3, CPU.reg.b);
}

/* 0xd9: Reset bit in register. */
static void set_3_c(void)
{
    CPU.reg.c = set(1 << 3, CPU.reg.c);
}

/* 0xda: Reset bit in register. */
static void set_3_d(void)
{
    CPU.reg.d = set(1 << 3, CPU.reg.d);
}

/* 0xdb: Reset bit in register. */
static void set_3_e(void)
{
    CPU.reg.e = set(1 << 3, CPU.reg.e);
}

/* 0xdc: Reset bit in register. */
static void set_3_h(void)
{
    CPU.reg.h = set(1 << 3, CPU.reg.h);
}

/* 0xdd: Reset bit in register. */
static void set_3_l(void)
{
    CPU.reg.l = set(1 << 3, CPU.reg.l);
}

/* 0xde: Reset bit in register. */
static void set_3_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 3, mmu_read_byte(CPU.reg.hl)));
}

/* 0xdf: Reset bit in register. */
static void set_3_a(void)
{
    CPU.reg.a = set(1 << 3, CPU.reg.a);
}

/* 0xe0: Reset bit in register. */
static void set_4_b(void)
{
    CPU.reg.b = set(1 << 4, CPU.reg.b);
}

/* 0xe1: Reset bit in register. */
static void set_4_c(void)
{
    CPU.reg.c = set(1 << 4, CPU.reg.c);
}

/* 0xe2: Reset bit in register. */
static void set_4_d(void)
{
    CPU.reg.d = set(1 << 4, CPU.reg.d);
}

/* 0xe3: Reset bit in register. */
static void set_4_e(void)
{
    CPU.reg.e = set(1 << 4, CPU.reg.e);
}

/* 0xe4: Reset bit in register. */
static void set_4_h(void)
{
    CPU.reg.h = set(1 << 4, CPU.reg.h);
}

/* 0xe5: Reset bit in register. */
static void set_4_l(void)
{
    CPU.reg.l = set(1 << 4, CPU.reg.l);
}

/* 0xe6: Reset bit in register. */
static void set_4_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 4, mmu_read_byte(CPU.reg.hl)));
}

/* 0xe7: Reset bit in register. */
static void set_4_a(void)
{
    CPU.reg.a = set(1 << 4, CPU.reg.a);
}

/* 0xe8: Reset bit in register. */
static void set_5_b(void)
{
    CPU.reg.b = set(1 << 5, CPU.reg.b);
}

/* 0xe9: Reset bit in register. */
static void set_5_c(void)
{
    CPU.reg.c = set(1 << 5, CPU.reg.c);
}

/* 0xea: Reset bit in register. */
static void set_5_d(void)
{
    CPU.reg.d = set(1 << 5, CPU.reg.d);
}

/* 0xeb: Reset bit in register. */
static void set_5_e(void)
{
    CPU.reg.e = set(1 << 5, CPU.reg.e);
}

/* 0xec: Reset bit in register. */
static void set_5_h(void)
{
    CPU.reg.h = set(1 << 5, CPU.reg.h);
}

/* 0xed: Reset bit in register. */
static void set_5_l(void)
{
    CPU.reg.l = set(1 << 5, CPU.reg.l);
}

/* 0xee: Reset bit in register. */
static void set_5_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 5, mmu_read_byte(CPU.reg.hl)));
}

/* 0xef: Reset bit in register. */
static void set_5_a(void)
{
    CPU.reg.a = set(1 << 5, CPU.reg.a);
}

/* 0xf0: Reset bit in register. */
static void set_6_b(void)
{
    CPU.reg.b = set(1 << 6, CPU.reg.b);
}

/* 0xf1: Reset bit in register. */
static void set_6_c(void)
{
    CPU.reg.c = set(1 << 6, CPU.reg.c);
}

/* 0xf2: Reset bit in register. */
static void set_6_d(void)
{
    CPU.reg.d = set(1 << 6, CPU.reg.d);
}

/* 0xf3: Reset bit in register. */
static void set_6_e(void)
{
    CPU.reg.e = set(1 << 6, CPU.reg.e);
}

/* 0xf4: Reset bit in register. */
static void set_6_h(void)
{
    CPU.reg.h = set(1 << 6, CPU.reg.h);
}

/* 0xf5: Reset bit in register. */
static void set_6_l(void)
{
    CPU.reg.l = set(1 << 6, CPU.reg.l);
}

/* 0xf6: Reset bit in register. */
static void set_6_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 6, mmu_read_byte(CPU.reg.hl)));
}

/* 0xf7: Reset bit in register. */
static void set_6_a(void)
{
    CPU.reg.a = set(1 << 6, CPU.reg.a);
}

/* 0xf8: Reset bit in register. */
static void set_7_b(void)
{
    CPU.reg.b = set(1 << 7, CPU.reg.b);
}

/* 0xf9: Reset bit in register. */
static void set_7_c(void)
{
    CPU.reg.c = set(1 << 7, CPU.reg.c);
}

/* 0xfa: Reset bit in register. */
static void set_7_d(void)
{
    CPU.reg.d = set(1 << 7, CPU.reg.d);
}

/* 0xfb: Reset bit in register. */
static void set_7_e(void)
{
    CPU.reg.e = set(1 << 7, CPU.reg.e);
}

/* 0xfc: Reset bit in register. */
static void set_7_h(void)
{
    CPU.reg.h = set(1 << 7, CPU.reg.h);
}

/* 0xfd: Reset bit in register. */
static void set_7_l(void)
{
    CPU.reg.l = set(1 << 7, CPU.reg.l);
}

/* 0xfe: Reset bit in register. */
static void set_7_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 7, mmu_read_byte(CPU.reg.hl)));
}

/* 0xff: Reset bit in register. */
static void set_7_a(void)
{
    CPU.reg.a = set(1 << 7, CPU.reg.a);
}

/* Extended operations. */
void cb_n(uint8_t opcode)
{
    switch (opcode) {
        case 0x00:
            rlc_b();
            break;
        case 0x01:
            rlc_c();
            break;
        case 0x02:
            rlc_d();
            break;
        case 0x03:
            rlc_e();
            break;
        case 0x04:
            rlc_h();
            break;
        case 0x05:
            rlc_l();
            break;
        case 0x06:
            rlc_hlp();
            break;
        case 0x07:
            rlc_a();
            break;
        case 0x08:
            rrc_b();
            break;
        case 0x09:
            rrc_c();
            break;
        case 0x0a:
            rrc_d();
            break;
        case 0x0b:
            rrc_e();
            break;
        case 0x0c:
            rrc_h();
            break;
        case 0x0d:
            rrc_l();
            break;
        case 0x0e:
            rrc_hlp();
            break;
        case 0x0f:
            rrc_a();
            break;
        case 0x10:
            rl_b();
            break;
        case 0x11:
            rl_c();
            break;
        case 0x12:
            rl_d();
            break;
        case 0x13:
            rl_e();
            break;
        case 0x14:
            rl_h();
            break;
        case 0x15:
            rl_l();
            break;
        case 0x16:
            rl_hlp();
            break;
        case 0x17:
            rl_a();
            break;
        case 0x18:
            rr_b();
            break;
        case 0x19:
            rr_c();
            break;
        case 0x1a:
            rr_d();
            break;
        case 0x1b:
            rr_e();
            break;
        case 0x1c:
            rr_h();
            break;
        case 0x1d:
            rr_l();
            break;
        case 0x1e:
            rr_hlp();
            break;
        case 0x1f:
            rr_a();
            break;
        case 0x20:
            sla_b();
            break;
        case 0x21:
            sla_c();
            break;
        case 0x22:
            sla_d();
            break;
        case 0x23:
            sla_e();
            break;
        case 0x24:
            sla_h();
            break;
        case 0x25:
            sla_l();
            break;
        case 0x26:
            sla_hlp();
            break;
        case 0x27:
            sla_a();
            break;
        case 0x28:
            sra_b();
            break;
        case 0x29:
            sra_c();
            break;
        case 0x2a:
            sra_d();
            break;
        case 0x2b:
            sra_e();
            break;
        case 0x2c:
            sra_h();
            break;
        case 0x2d:
            sra_l();
            break;
        case 0x2e:
            sra_hlp();
            break;
        case 0x2f:
            sra_a();
            break;
        case 0x30:
            swap_b();
            break;
        case 0x31:
            swap_c();
            break;
        case 0x32:
            swap_d();
            break;
        case 0x33:
            swap_e();
            break;
        case 0x34:
            swap_h();
            break;
        case 0x35:
            swap_l();
            break;
        case 0x36:
            swap_hlp();
            break;
        case 0x37:
            swap_a();
            break;
        case 0x38:
            srl_b();
            break;
        case 0x39:
            srl_c();
            break;
        case 0x3a:
            srl_d();
            break;
        case 0x3b:
            srl_e();
            break;
        case 0x3c:
            srl_h();
            break;
        case 0x3d:
            srl_l();
            break;
        case 0x3e:
            srl_hlp();
            break;
        case 0x3f:
            srl_a();
            break;
        case 0x40:
            bit_0_b();
            break;
        case 0x41:
            bit_0_c();
            break;
        case 0x42:
            bit_0_d();
            break;
        case 0x43:
            bit_0_e();
            break;
        case 0x44:
            bit_0_h();
            break;
        case 0x45:
            bit_0_l();
            break;
        case 0x46:
            bit_0_hlp();
            break;
        case 0x47:
            bit_0_a();
            break;
        case 0x48:
            bit_1_b();
            break;
        case 0x49:
            bit_1_c();
            break;
        case 0x4a:
            bit_1_d();
            break;
        case 0x4b:
            bit_1_e();
            break;
        case 0x4c:
            bit_1_h();
            break;
        case 0x4d:
            bit_1_l();
            break;
        case 0x4e:
            bit_1_hlp();
            break;
        case 0x4f:
            bit_1_a();
            break;
        case 0x50:
            bit_2_b();
            break;
        case 0x51:
            bit_2_c();
            break;
        case 0x52:
            bit_2_d();
            break;
        case 0x53:
            bit_2_e();
            break;
        case 0x54:
            bit_2_h();
            break;
        case 0x55:
            bit_2_l();
            break;
        case 0x56:
            bit_2_hlp();
            break;
        case 0x57:
            bit_2_a();
            break;
        case 0x58:
            bit_3_b();
            break;
        case 0x59:
            bit_3_c();
            break;
        case 0x5a:
            bit_3_d();
            break;
        case 0x5b:
            bit_3_e();
            break;
        case 0x5c:
            bit_3_h();
            break;
        case 0x5d:
            bit_3_l();
            break;
        case 0x5e:
            bit_3_hlp();
            break;
        case 0x5f:
            bit_3_a();
            break;
        case 0x60:
            bit_4_b();
            break;
        case 0x61:
            bit_4_c();
            break;
        case 0x62:
            bit_4_d();
            break;
        case 0x63:
            bit_4_e();
            break;
        case 0x64:
            bit_4_h();
            break;
        case 0x65:
            bit_4_l();
            break;
        case 0x66:
            bit_4_hlp();
            break;
        case 0x67:
            bit_4_a();
            break;
        case 0x68:
            bit_5_b();
            break;
        case 0x69:
            bit_5_c();
            break;
        case 0x6a:
            bit_5_d();
            break;
        case 0x6b:
            bit_5_e();
            break;
        case 0x6c:
            bit_5_h();
            break;
        case 0x6d:
            bit_5_l();
            break;
        case 0x6e:
            bit_5_hlp();
            break;
        case 0x6f:
            bit_5_a();
            break;
        case 0x70:
            bit_6_b();
            break;
        case 0x71:
            bit_6_c();
            break;
        case 0x72:
            bit_6_d();
            break;
        case 0x73:
            bit_6_e();
            break;
        case 0x74:
            bit_6_h();
            break;
        case 0x75:
            bit_6_l();
            break;
        case 0x76:
            bit_6_hlp();
            break;
        case 0x77:
            bit_6_a();
            break;
        case 0x78:
            bit_7_b();
            break;
        case 0x79:
            bit_7_c();
            break;
        case 0x7a:
            bit_7_d();
            break;
        case 0x7b:
            bit_7_e();
            break;
        case 0x7c:
            bit_7_h();
            break;
        case 0x7d:
            bit_7_l();
            break;
        case 0x7e:
            bit_7_hlp();
            break;
        case 0x7f:
            bit_7_a();
            break;
        case 0x80:
            res_0_b();
            break;
        case 0x81:
            res_0_c();
            break;
        case 0x82:
            res_0_d();
            break;
        case 0x83:
            res_0_e();
            break;
        case 0x84:
            res_0_h();
            break;
        case 0x85:
            res_0_l();
            break;
        case 0x86:
            res_0_hlp();
            break;
        case 0x87:
            res_0_a();
            break;
        case 0x88:
            res_1_b();
            break;
        case 0x89:
            res_1_c();
            break;
        case 0x8a:
            res_1_d();
            break;
        case 0x8b:
            res_1_e();
            break;
        case 0x8c:
            res_1_h();
            break;
        case 0x8d:
            res_1_l();
            break;
        case 0x8e:
            res_1_hlp();
            break;
        case 0x8f:
            res_1_a();
            break;
        case 0x90:
            res_2_b();
            break;
        case 0x91:
            res_2_c();
            break;
        case 0x92:
            res_2_d();
            break;
        case 0x93:
            res_2_e();
            break;
        case 0x94:
            res_2_h();
            break;
        case 0x95:
            res_2_l();
            break;
        case 0x96:
            res_2_hlp();
            break;
        case 0x97:
            res_2_a();
            break;
        case 0x98:
            res_3_b();
            break;
        case 0x99:
            res_3_c();
            break;
        case 0x9a:
            res_3_d();
            break;
        case 0x9b:
            res_3_e();
            break;
        case 0x9c:
            res_3_h();
            break;
        case 0x9d:
            res_3_l();
            break;
        case 0x9e:
            res_3_hlp();
            break;
        case 0x9f:
            res_3_a();
            break;
        case 0xa0:
            res_4_b();
            break;
        case 0xa1:
            res_4_c();
            break;
        case 0xa2:
            res_4_d();
            break;
        case 0xa3:
            res_4_e();
            break;
        case 0xa4:
            res_4_h();
            break;
        case 0xa5:
            res_4_l();
            break;
        case 0xa6:
            res_4_hlp();
            break;
        case 0xa7:
            res_4_a();
            break;
        case 0xa8:
            res_5_b();
            break;
        case 0xa9:
            res_5_c();
            break;
        case 0xaa:
            res_5_d();
            break;
        case 0xab:
            res_5_e();
            break;
        case 0xac:
            res_5_h();
            break;
        case 0xad:
            res_5_l();
            break;
        case 0xae:
            res_5_hlp();
            break;
        case 0xaf:
            res_5_a();
            break;
        case 0xb0:
            res_6_b();
            break;
        case 0xb1:
            res_6_c();
            break;
        case 0xb2:
            res_6_d();
            break;
        case 0xb3:
            res_6_e();
            break;
        case 0xb4:
            res_6_h();
            break;
        case 0xb5:
            res_6_l();
            break;
        case 0xb6:
            res_6_hlp();
            break;
        case 0xb7:
            res_6_a();
            break;
        case 0xb8:
            res_7_b();
            break;
        case 0xb9:
            res_7_c();
            break;
        case 0xba:
            res_7_d();
            break;
        case 0xbb:
            res_7_e();
            break;
        case 0xbc:
            res_7_h();
            break;
        case 0xbd:
            res_7_l();
            break;
        case 0xbe:
            res_7_hlp();
            break;
        case 0xbf:
            res_7_a();
            break;
        case 0xc0:
            set_0_b();
            break;
        case 0xc1:
            set_0_c();
            break;
        case 0xc2:
            set_0_d();
            break;
        case 0xc3:
            set_0_e();
            break;
        case 0xc4:
            set_0_h();
            break;
        case 0xc5:
            set_0_l();
            break;
        case 0xc6:
            set_0_hlp();
            break;
        case 0xc7:
            set_0_a();
            break;
        case 0xc8:
            set_1_b();
            break;
        case 0xc9:
            set_1_c();
            break;
        case 0xca:
            set_1_d();
            break;
        case 0xcb:
            set_1_e();
            break;
        case 0xcc:
            set_1_h();
            break;
        case 0xcd:
            set_1_l();
            break;
        case 0xce:
            set_1_hlp();
            break;
        case 0xcf:
            set_1_a();
            break;
        case 0xd0:
            set_2_b();
            break;
        case 0xd1:
            set_2_c();
            break;
        case 0xd2:
            set_2_d();
            break;
        case 0xd3:
            set_2_e();
            break;
        case 0xd4:
            set_2_h();
            break;
        case 0xd5:
            set_2_l();
            break;
        case 0xd6:
            set_2_hlp();
            break;
        case 0xd7:
            set_2_a();
            break;
        case 0xd8:
            set_3_b();
            break;
        case 0xd9:
            set_3_c();
            break;
        case 0xda:
            set_3_d();
            break;
        case 0xdb:
            set_3_e();
            break;
        case 0xdc:
            set_3_h();
            break;
        case 0xdd:
            set_3_l();
            break;
        case 0xde:
            set_3_hlp();
            break;
        case 0xdf:
            set_3_a();
            break;
        case 0xe0:
            set_4_b();
            break;
        case 0xe1:
            set_4_c();
            break;
        case 0xe2:
            set_4_d();
            break;
        case 0xe3:
            set_4_e();
            break;
        case 0xe4:
            set_4_h();
            break;
        case 0xe5:
            set_4_l();
            break;
        case 0xe6:
            set_4_hlp();
            break;
        case 0xe7:
            set_4_a();
            break;
        case 0xe8:
            set_5_b();
            break;
        case 0xe9:
            set_5_c();
            break;
        case 0xea:
            set_5_d();
            break;
        case 0xeb:
            set_5_e();
            break;
        case 0xec:
            set_5_h();
            break;
        case 0xed:
            set_5_l();
            break;
        case 0xee:
            set_5_hlp();
            break;
        case 0xef:
            set_5_a();
            break;
        case 0xf0:
            set_6_b();
            break;
        case 0xf1:
            set_6_c();
            break;
        case 0xf2:
            set_6_d();
            break;
        case 0xf3:
            set_6_e();
            break;
        case 0xf4:
            set_6_h();
            break;
        case 0xf5:
            set_6_l();
            break;
        case 0xf6:
            set_6_hlp();
            break;
        case 0xf7:
            set_6_a();
            break;
        case 0xf8:
            set_7_b();
            break;
        case 0xf9:
            set_7_c();
            break;
        case 0xfa:
            set_7_d();
            break;
        case 0xfb:
            set_7_e();
            break;
        case 0xfc:
            set_7_h();
            break;
        case 0xfd:
            set_7_l();
            break;
        case 0xfe:
            set_7_hlp();
            break;
        case 0xff:
            set_7_a();
            break;
    }
}
