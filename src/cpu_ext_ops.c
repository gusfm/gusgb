#include "cpu_ext_ops.h"
#include <stdio.h>
#include "clock.h"
#include "cpu.h"
#include "cpu_utils.h"
#include "mmu.h"

extern cpu_t CPU;

typedef struct {
    const char *asm1;
    void (*execute)(void);
} ext_instruction_t;

const ext_instruction_t g_ext_instr[256] = {
    {"rlc b", rlc_b},           // 0x00
    {"rlc c", rlc_c},           // 0x01
    {"rlc d", rlc_d},           // 0x02
    {"rlc e", rlc_e},           // 0x03
    {"rlc h", rlc_h},           // 0x04
    {"rlc l", rlc_l},           // 0x05
    {"rlc (hl)", rlc_hlp},      // 0x06
    {"rlc a", rlc_a},           // 0x07
    {"rrc b", rrc_b},           // 0x08
    {"rrc c", rrc_c},           // 0x09
    {"rrc d", rrc_d},           // 0x0a
    {"rrc e", rrc_e},           // 0x0b
    {"rrc h", rrc_h},           // 0x0c
    {"rrc l", rrc_l},           // 0x0d
    {"rrc (hl)", rrc_hlp},      // 0x0e
    {"rrc a", rrc_a},           // 0x0f
    {"rl b", rl_b},             // 0x10
    {"rl c", rl_c},             // 0x11
    {"rl d", rl_d},             // 0x12
    {"rl e", rl_e},             // 0x13
    {"rl h", rl_h},             // 0x14
    {"rl l", rl_l},             // 0x15
    {"rl (hl)", rl_hlp},        // 0x16
    {"rl a", rl_a},             // 0x17
    {"rr b", rr_b},             // 0x18
    {"rr c", rr_c},             // 0x19
    {"rr d", rr_d},             // 0x1a
    {"rr e", rr_e},             // 0x1b
    {"rr h", rr_h},             // 0x1c
    {"rr l", rr_l},             // 0x1d
    {"rr (hl)", rr_hlp},        // 0x1e
    {"rr a", rr_a},             // 0x1f
    {"sla b", sla_b},           // 0x20
    {"sla c", sla_c},           // 0x21
    {"sla d", sla_d},           // 0x22
    {"sla e", sla_e},           // 0x23
    {"sla h", sla_h},           // 0x24
    {"sla l", sla_l},           // 0x25
    {"sla (hl)", sla_hlp},      // 0x26
    {"sla a", sla_a},           // 0x27
    {"sra b", sra_b},           // 0x28
    {"sra c", sra_c},           // 0x29
    {"sra d", sra_d},           // 0x2a
    {"sra e", sra_e},           // 0x2b
    {"sra h", sra_h},           // 0x2c
    {"sra l", sra_l},           // 0x2d
    {"sra (hl)", sra_hlp},      // 0x2e
    {"sra a", sra_a},           // 0x2f
    {"swap b", swap_b},         // 0x30
    {"swap c", swap_c},         // 0x31
    {"swap d", swap_d},         // 0x32
    {"swap e", swap_e},         // 0x33
    {"swap h", swap_h},         // 0x34
    {"swap l", swap_l},         // 0x35
    {"swap (hl)", swap_hlp},    // 0x36
    {"swap a", swap_a},         // 0x37
    {"srl b", srl_b},           // 0x38
    {"srl c", srl_c},           // 0x39
    {"srl d", srl_d},           // 0x3a
    {"srl e", srl_e},           // 0x3b
    {"srl h", srl_h},           // 0x3c
    {"srl l", srl_l},           // 0x3d
    {"srl (hl)", srl_hlp},      // 0x3e
    {"srl a", srl_a},           // 0x3f
    {"bit 0, b", bit_0_b},      // 0x40
    {"bit 0, c", bit_0_c},      // 0x41
    {"bit 0, d", bit_0_d},      // 0x42
    {"bit 0, e", bit_0_e},      // 0x43
    {"bit 0, h", bit_0_h},      // 0x44
    {"bit 0, l", bit_0_l},      // 0x45
    {"bit 0, (hl)", bit_0_hlp}, // 0x46
    {"bit 0, a", bit_0_a},      // 0x47
    {"bit 1, b", bit_1_b},      // 0x48
    {"bit 1, c", bit_1_c},      // 0x49
    {"bit 1, d", bit_1_d},      // 0x4a
    {"bit 1, e", bit_1_e},      // 0x4b
    {"bit 1, h", bit_1_h},      // 0x4c
    {"bit 1, l", bit_1_l},      // 0x4d
    {"bit 1, (hl)", bit_1_hlp}, // 0x4e
    {"bit 1, a", bit_1_a},      // 0x4f
    {"bit 2, b", bit_2_b},      // 0x50
    {"bit 2, c", bit_2_c},      // 0x51
    {"bit 2, d", bit_2_d},      // 0x52
    {"bit 2, e", bit_2_e},      // 0x53
    {"bit 2, h", bit_2_h},      // 0x54
    {"bit 2, l", bit_2_l},      // 0x55
    {"bit 2, (hl)", bit_2_hlp}, // 0x56
    {"bit 2, a", bit_2_a},      // 0x57
    {"bit 3, b", bit_3_b},      // 0x58
    {"bit 3, c", bit_3_c},      // 0x59
    {"bit 3, d", bit_3_d},      // 0x5a
    {"bit 3, e", bit_3_e},      // 0x5b
    {"bit 3, h", bit_3_h},      // 0x5c
    {"bit 3, l", bit_3_l},      // 0x5d
    {"bit 3, (hl)", bit_3_hlp}, // 0x5e
    {"bit 3, a", bit_3_a},      // 0x5f
    {"bit 4, b", bit_4_b},      // 0x60
    {"bit 4, c", bit_4_c},      // 0x61
    {"bit 4, d", bit_4_d},      // 0x62
    {"bit 4, e", bit_4_e},      // 0x63
    {"bit 4, h", bit_4_h},      // 0x64
    {"bit 4, l", bit_4_l},      // 0x65
    {"bit 4, (hl)", bit_4_hlp}, // 0x66
    {"bit 4, a", bit_4_a},      // 0x67
    {"bit 5, b", bit_5_b},      // 0x68
    {"bit 5, c", bit_5_c},      // 0x69
    {"bit 5, d", bit_5_d},      // 0x6a
    {"bit 5, e", bit_5_e},      // 0x6b
    {"bit 6, h", bit_5_h},      // 0x6c
    {"bit 6, l", bit_5_l},      // 0x6d
    {"bit 5, (hl)", bit_5_hlp}, // 0x6e
    {"bit 5, a", bit_5_a},      // 0x6f
    {"bit 6, b", bit_6_b},      // 0x70
    {"bit 6, c", bit_6_c},      // 0x71
    {"bit 6, d", bit_6_d},      // 0x72
    {"bit 6, e", bit_6_e},      // 0x73
    {"bit 6, h", bit_6_h},      // 0x74
    {"bit 6, l", bit_6_l},      // 0x75
    {"bit 6, (hl)", bit_6_hlp}, // 0x76
    {"bit 6, a", bit_6_a},      // 0x77
    {"bit 7, b", bit_7_b},      // 0x78
    {"bit 7, c", bit_7_c},      // 0x79
    {"bit 7, d", bit_7_d},      // 0x7a
    {"bit 7, e", bit_7_e},      // 0x7b
    {"bit 7, h", bit_7_h},      // 0x7c
    {"bit 7, l", bit_7_l},      // 0x7d
    {"bit 7, (hl)", bit_7_hlp}, // 0x7e
    {"bit 7, a", bit_7_a},      // 0x7f
    {"res 0, b", res_0_b},      // 0x80
    {"res 0, c", res_0_c},      // 0x81
    {"res 0, d", res_0_d},      // 0x82
    {"res 0, e", res_0_e},      // 0x83
    {"res 0, h", res_0_h},      // 0x84
    {"res 0, l", res_0_l},      // 0x85
    {"res 0, (hl)", res_0_hlp}, // 0x86
    {"res 0, a", res_0_a},      // 0x87
    {"res 1, b", res_1_b},      // 0x88
    {"res 1, c", res_1_c},      // 0x89
    {"res 1, d", res_1_d},      // 0x8a
    {"res 1, e", res_1_e},      // 0x8b
    {"res 1, h", res_1_h},      // 0x8c
    {"res 1, l", res_1_l},      // 0x8d
    {"res 1, (hl)", res_1_hlp}, // 0x8e
    {"res 1, a", res_1_a},      // 0x8f
    {"res 2, b", res_2_b},      // 0x90
    {"res 2, c", res_2_c},      // 0x91
    {"res 2, d", res_2_d},      // 0x92
    {"res 2, e", res_2_e},      // 0x93
    {"res 2, h", res_2_h},      // 0x94
    {"res 2, l", res_2_l},      // 0x95
    {"res 2, (hl)", res_2_hlp}, // 0x96
    {"res 2, a", res_2_a},      // 0x97
    {"res 3, b", res_3_b},      // 0x98
    {"res 3, c", res_3_c},      // 0x99
    {"res 3, d", res_3_d},      // 0x9a
    {"res 3, e", res_3_e},      // 0x9b
    {"res 3, h", res_3_h},      // 0x9c
    {"res 3, l", res_3_l},      // 0x9d
    {"res 3, (hl)", res_3_hlp}, // 0x9e
    {"res 3, a", res_3_a},      // 0x9f
    {"res 4, b", res_4_b},      // 0xa0
    {"res 4, c", res_4_c},      // 0xa1
    {"res 4, d", res_4_d},      // 0xa2
    {"res 4, e", res_4_e},      // 0xa3
    {"res 4, h", res_4_h},      // 0xa4
    {"res 4, l", res_4_l},      // 0xa5
    {"res 4, (hl)", res_4_hlp}, // 0xa6
    {"res 4, a", res_4_a},      // 0xa7
    {"res 5, b", res_5_b},      // 0xa8
    {"res 5, c", res_5_c},      // 0xa9
    {"res 5, d", res_5_d},      // 0xaa
    {"res 5, e", res_5_e},      // 0xab
    {"res 5, h", res_5_h},      // 0xac
    {"res 5, l", res_5_l},      // 0xad
    {"res 5, (hl)", res_5_hlp}, // 0xae
    {"res 5, a", res_5_a},      // 0xaf
    {"res 6, b", res_6_b},      // 0xb0
    {"res 6, c", res_6_c},      // 0xb1
    {"res 6, d", res_6_d},      // 0xb2
    {"res 6, e", res_6_e},      // 0xb3
    {"res 6, h", res_6_h},      // 0xb4
    {"res 6, l", res_6_l},      // 0xb5
    {"res 6, (hl)", res_6_hlp}, // 0xb6
    {"res 6, a", res_6_a},      // 0xb7
    {"res 7, b", res_7_b},      // 0xb8
    {"res 7, c", res_7_c},      // 0xb9
    {"res 7, d", res_7_d},      // 0xba
    {"res 7, e", res_7_e},      // 0xbb
    {"res 7, h", res_7_h},      // 0xbc
    {"res 7, l", res_7_l},      // 0xbd
    {"res 7, (hl)", res_7_hlp}, // 0xbe
    {"res 7, a", res_7_a},      // 0xbf
    {"set 0, b", set_0_b},      // 0xc0
    {"set 0, c", set_0_c},      // 0xc1
    {"set 0, d", set_0_d},      // 0xc2
    {"set 0, e", set_0_e},      // 0xc3
    {"set 0, h", set_0_h},      // 0xc4
    {"set 0, l", set_0_l},      // 0xc5
    {"set 0, (hl)", set_0_hlp}, // 0xc6
    {"set 0, a", set_0_a},      // 0xc7
    {"set 1, b", set_1_b},      // 0xc8
    {"set 1, c", set_1_c},      // 0xc9
    {"set 1, d", set_1_d},      // 0xca
    {"set 1, e", set_1_e},      // 0xcb
    {"set 1, h", set_1_h},      // 0xcc
    {"set 1, l", set_1_l},      // 0xcd
    {"set 1, (hl)", set_1_hlp}, // 0xce
    {"set 1, a", set_1_a},      // 0xcf
    {"set 2, b", set_2_b},      // 0xd0
    {"set 2, c", set_2_c},      // 0xd1
    {"set 2, d", set_2_d},      // 0xd2
    {"set 2, e", set_2_e},      // 0xd3
    {"set 2, h", set_2_h},      // 0xd4
    {"set 2, l", set_2_l},      // 0xd5
    {"set 2, (hl)", set_2_hlp}, // 0xd6
    {"set 2, a", set_2_a},      // 0xd7
    {"set 3, b", set_3_b},      // 0xd8
    {"set 3, c", set_3_c},      // 0xd9
    {"set 3, d", set_3_d},      // 0xda
    {"set 3, e", set_3_e},      // 0xdb
    {"set 3, h", set_3_h},      // 0xdc
    {"set 3, l", set_3_l},      // 0xdd
    {"set 3, (hl)", set_3_hlp}, // 0xde
    {"set 3, a", set_3_a},      // 0xdf
    {"set 4, b", set_4_b},      // 0xe0
    {"set 4, c", set_4_c},      // 0xe1
    {"set 4, d", set_4_d},      // 0xe2
    {"set 4, e", set_4_e},      // 0xe3
    {"set 4, h", set_4_h},      // 0xe4
    {"set 4, l", set_4_l},      // 0xe5
    {"set 4, (hl)", set_4_hlp}, // 0xe6
    {"set 4, a", set_4_a},      // 0xe7
    {"set 5, b", set_5_b},      // 0xe8
    {"set 5, c", set_5_c},      // 0xe9
    {"set 5, d", set_5_d},      // 0xea
    {"set 5, e", set_5_e},      // 0xeb
    {"set 5, h", set_5_h},      // 0xec
    {"set 5, l", set_5_l},      // 0xed
    {"set 5, (hl)", set_5_hlp}, // 0xee
    {"set 5, a", set_5_a},      // 0xef
    {"set 6, b", set_6_b},      // 0xf0
    {"set 6, c", set_6_c},      // 0xf1
    {"set 6, d", set_6_d},      // 0xf2
    {"set 6, e", set_6_e},      // 0xf3
    {"set 6, h", set_6_h},      // 0xf4
    {"set 6, l", set_6_l},      // 0xf5
    {"set 6, (hl)", set_6_hlp}, // 0xf6
    {"set 6, a", set_6_a},      // 0xf7
    {"set 7, b", set_7_b},      // 0xf8
    {"set 7, c", set_7_c},      // 0xf9
    {"set 7, d", set_7_d},      // 0xfa
    {"set 7, e", set_7_e},      // 0xfb
    {"set 7, h", set_7_h},      // 0xfc
    {"set 7, l", set_7_l},      // 0xfd
    {"set 7, (hl)", set_7_hlp}, // 0xfe
    {"set 7, a", set_7_a},      // 0xff
};

void print_ext_ops(char *str, uint8_t opcode)
{
    sprintf(str, "0xcb%02x: %s", opcode, g_ext_instr[opcode].asm1);
}

/* 0xcb: Extended operations. */
void cb_n(uint8_t opcode)
{
    g_ext_instr[opcode].execute();
}

/* 0x00: Rotate B with carry. */
void rlc_b(void)
{
    CPU.reg.b = rlc(CPU.reg.b);
}

/* 0x01: Rotate C with carry. */
void rlc_c(void)
{
    CPU.reg.c = rlc(CPU.reg.c);
}

/* 0x02: Rotate D with carry. */
void rlc_d(void)
{
    CPU.reg.d = rlc(CPU.reg.d);
}

/* 0x03: Rotate E with carry. */
void rlc_e(void)
{
    CPU.reg.e = rlc(CPU.reg.e);
}

/* 0x04: Rotate H with carry. */
void rlc_h(void)
{
    CPU.reg.h = rlc(CPU.reg.h);
}

/* 0x05: Rotate L with carry. */
void rlc_l(void)
{
    CPU.reg.l = rlc(CPU.reg.l);
}

/* 0x06: Rotate (HL) with carry. */
void rlc_hlp(void)
{
    uint8_t val = rlc(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x07: Rotate A with carry. */
void rlc_a(void)
{
    CPU.reg.a = rlc(CPU.reg.a);
}

/* 0x08: Rotate B with carry. */
void rrc_b(void)
{
    CPU.reg.b = rrc(CPU.reg.b);
}

/* 0x09: Rotate C with carry. */
void rrc_c(void)
{
    CPU.reg.c = rrc(CPU.reg.c);
}

/* 0x0a: Rotate D with carry. */
void rrc_d(void)
{
    CPU.reg.d = rrc(CPU.reg.d);
}

/* 0x0b: Rotate E with carry. */
void rrc_e(void)
{
    CPU.reg.e = rrc(CPU.reg.e);
}

/* 0x0c: Rotate H with carry. */
void rrc_h(void)
{
    CPU.reg.h = rrc(CPU.reg.h);
}

/* 0x0d: Rotate L with carry. */
void rrc_l(void)
{
    CPU.reg.l = rrc(CPU.reg.l);
}

/* 0x0e: Rotate (HL) with carry. */
void rrc_hlp(void)
{
    uint8_t val = rrc(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x0f: Rotate A with carry. */
void rrc_a(void)
{
    CPU.reg.a = rrc(CPU.reg.a);
}

/* 0x10: Rotate B left through Carry flag. */
void rl_b(void)
{
    CPU.reg.b = rl(CPU.reg.b);
}

/* 0x11: Rotate C left through Carry flag. */
void rl_c(void)
{
    CPU.reg.c = rl(CPU.reg.c);
}

/* 0x12: Rotate D left through Carry flag. */
void rl_d(void)
{
    CPU.reg.d = rl(CPU.reg.d);
}

/* 0x13: Rotate E left through Carry flag. */
void rl_e(void)
{
    CPU.reg.e = rl(CPU.reg.e);
}

/* 0x14: Rotate H left through Carry flag. */
void rl_h(void)
{
    CPU.reg.h = rl(CPU.reg.h);
}

/* 0x15: Rotate L left through Carry flag. */
void rl_l(void)
{
    CPU.reg.l = rl(CPU.reg.l);
}

/* 0x16: Rotate (HL) with carry. */
void rl_hlp(void)
{
    uint8_t val = rl(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x17: Rotate A left through Carry flag. */
void rl_a(void)
{
    CPU.reg.a = rl(CPU.reg.a);
}

/* 0x18: Rotate B right through carry flag. */
void rr_b(void)
{
    CPU.reg.b = rr(CPU.reg.b);
}

/* 0x19: Rotate C right through carry flag. */
void rr_c(void)
{
    CPU.reg.c = rr(CPU.reg.c);
}

/* 0x1a: Rotate D right through carry flag. */
void rr_d(void)
{
    CPU.reg.d = rr(CPU.reg.d);
}

/* 0x1b: Rotate E right through carry flag. */
void rr_e(void)
{
    CPU.reg.e = rr(CPU.reg.e);
}

/* 0x1c: Rotate H right through carry flag. */
void rr_h(void)
{
    CPU.reg.h = rr(CPU.reg.h);
}

/* 0x1d: Rotate L right through carry flag. */
void rr_l(void)
{
    CPU.reg.l = rr(CPU.reg.l);
}

/* 0x1e: Rotate (HL) right through carry flag. */
void rr_hlp(void)
{
    uint8_t val = rr(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x1f: Rotate A right through carry flag. */
void rr_a(void)
{
    CPU.reg.a = rr(CPU.reg.a);
}

/* 0x20: Shift B left into Carry flag. */
void sla_b(void)
{
    CPU.reg.b = sla(CPU.reg.b);
}

/* 0x21: Shift C left into Carry flag. */
void sla_c(void)
{
    CPU.reg.c = sla(CPU.reg.c);
}

/* 0x22: Shift D left into Carry flag. */
void sla_d(void)
{
    CPU.reg.d = sla(CPU.reg.d);
}

/* 0x23: Shift E left into Carry flag. */
void sla_e(void)
{
    CPU.reg.e = sla(CPU.reg.e);
}

/* 0x24: Shift H left into Carry flag. */
void sla_h(void)
{
    CPU.reg.h = sla(CPU.reg.h);
}

/* 0x25: Shift L left into Carry flag. */
void sla_l(void)
{
    CPU.reg.l = sla(CPU.reg.l);
}

/* 0x26: Shift (HL) with carry. */
void sla_hlp(void)
{
    uint8_t val = sla(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x27: Shift A left into Carry flag. */
void sla_a(void)
{
    CPU.reg.a = sla(CPU.reg.a);
}

/* 0x28: Shift B right into Carry flag. */
void sra_b(void)
{
    CPU.reg.b = sra(CPU.reg.b);
}

/* 0x29: Shift C right into Carry flag. */
void sra_c(void)
{
    CPU.reg.c = sra(CPU.reg.c);
}

/* 0x2a: Shift D right into Carry flag. */
void sra_d(void)
{
    CPU.reg.d = sra(CPU.reg.d);
}

/* 0x2b: Shift E right into Carry flag. */
void sra_e(void)
{
    CPU.reg.e = sra(CPU.reg.e);
}

/* 0x2c: Shift H right into Carry flag. */
void sra_h(void)
{
    CPU.reg.h = sra(CPU.reg.h);
}

/* 0x2d: Shift L right into Carry flag. */
void sra_l(void)
{
    CPU.reg.l = sra(CPU.reg.l);
}

/* 0x2e: Shift (HL) right into Carry flag. */
void sra_hlp(void)
{
    uint8_t val = sra(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x2f: Shift A right into Carry flag. */
void sra_a(void)
{
    CPU.reg.a = sra(CPU.reg.a);
}

/* 0x30: Swap upper & lower nibbles of n. */
void swap_b(void)
{
    CPU.reg.b = swap(CPU.reg.b);
}

/* 0x31: Swap upper & lower nibbles of n. */
void swap_c(void)
{
    CPU.reg.c = swap(CPU.reg.c);
}

/* 0x32: Swap upper & lower nibbles of n. */
void swap_d(void)
{
    CPU.reg.d = swap(CPU.reg.d);
}

/* 0x33: Swap upper & lower nibbles of n. */
void swap_e(void)
{
    CPU.reg.e = swap(CPU.reg.e);
}

/* 0x34: Swap upper & lower nibbles of n. */
void swap_h(void)
{
    CPU.reg.h = swap(CPU.reg.h);
}

/* 0x35: Swap upper & lower nibbles of n. */
void swap_l(void)
{
    CPU.reg.l = swap(CPU.reg.l);
}

/* 0x36: Swap upper & lower nibbles of n. */
void swap_hlp(void)
{
    uint8_t val = swap(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x37: Swap upper & lower nibbles of n. */
void swap_a(void)
{
    CPU.reg.a = swap(CPU.reg.a);
}

/* 0x38: Shift B right into Carry flag. */
void srl_b(void)
{
    CPU.reg.b = srl(CPU.reg.b);
}

/* 0x39: Shift C right into Carry flag. */
void srl_c(void)
{
    CPU.reg.c = srl(CPU.reg.c);
}

/* 0x3a: Shift D right into Carry flag. */
void srl_d(void)
{
    CPU.reg.d = srl(CPU.reg.d);
}

/* 0x3b: Shift E right into Carry flag. */
void srl_e(void)
{
    CPU.reg.e = srl(CPU.reg.e);
}

/* 0x3c: Shift H right into Carry flag. */
void srl_h(void)
{
    CPU.reg.h = srl(CPU.reg.h);
}

/* 0x3d: Shift L right into Carry flag. */
void srl_l(void)
{
    CPU.reg.l = srl(CPU.reg.l);
}

/* 0x3e: Shift (HL) right into Carry flag. */
void srl_hlp(void)
{
    uint8_t val = srl(mmu_read_byte(CPU.reg.hl));
    mmu_write_byte(CPU.reg.hl, val);
}

/* 0x3f: Shift A right into Carry flag. */
void srl_a(void)
{
    CPU.reg.a = srl(CPU.reg.a);
}

/* 0x40: Test bit in register. */
void bit_0_b(void)
{
    bit(1 << 0, CPU.reg.b);
}

/* 0x41: Test bit in register. */
void bit_0_c(void)
{
    bit(1 << 0, CPU.reg.c);
}

/* 0x42: Test bit in register. */
void bit_0_d(void)
{
    bit(1 << 0, CPU.reg.d);
}

/* 0x43: Test bit in register. */
void bit_0_e(void)
{
    bit(1 << 0, CPU.reg.e);
}

/* 0x44: Test bit in register. */
void bit_0_h(void)
{
    bit(1 << 0, CPU.reg.h);
}

/* 0x45: Test bit in register. */
void bit_0_l(void)
{
    bit(1 << 0, CPU.reg.l);
}

/* 0x46: Test bit in register. */
void bit_0_hlp(void)
{
    bit(1 << 0, mmu_read_byte(CPU.reg.hl));
}

/* 0x47: Test bit in register. */
void bit_0_a(void)
{
    bit(1 << 0, CPU.reg.a);
}

/* 0x48: Test bit in register. */
void bit_1_b(void)
{
    bit(1 << 1, CPU.reg.b);
}

/* 0x49: Test bit in register. */
void bit_1_c(void)
{
    bit(1 << 1, CPU.reg.c);
}

/* 0x4a: Test bit in register. */
void bit_1_d(void)
{
    bit(1 << 1, CPU.reg.d);
}

/* 0x4b: Test bit in register. */
void bit_1_e(void)
{
    bit(1 << 1, CPU.reg.e);
}

/* 0x4c: Test bit in register. */
void bit_1_h(void)
{
    bit(1 << 1, CPU.reg.h);
}

/* 0x4d: Test bit in register. */
void bit_1_l(void)
{
    bit(1 << 1, CPU.reg.l);
}

/* 0x4e: Test bit in register. */
void bit_1_hlp(void)
{
    bit(1 << 1, mmu_read_byte(CPU.reg.hl));
}

/* 0x4f: Test bit in register. */
void bit_1_a(void)
{
    bit(1 << 1, CPU.reg.a);
}

/* 0x50: Test bit in register. */
void bit_2_b(void)
{
    bit(1 << 2, CPU.reg.b);
}

/* 0x51: Test bit in register. */
void bit_2_c(void)
{
    bit(1 << 2, CPU.reg.c);
}

/* 0x52: Test bit in register. */
void bit_2_d(void)
{
    bit(1 << 2, CPU.reg.d);
}

/* 0x53: Test bit in register. */
void bit_2_e(void)
{
    bit(1 << 2, CPU.reg.e);
}

/* 0x54: Test bit in register. */
void bit_2_h(void)
{
    bit(1 << 2, CPU.reg.h);
}

/* 0x55: Test bit in register. */
void bit_2_l(void)
{
    bit(1 << 2, CPU.reg.l);
}

/* 0x56: Test bit in register. */
void bit_2_hlp(void)
{
    bit(1 << 2, mmu_read_byte(CPU.reg.hl));
}

/* 0x57: Test bit in register. */
void bit_2_a(void)
{
    bit(1 << 2, CPU.reg.a);
}

/* 0x58: Test bit in register. */
void bit_3_b(void)
{
    bit(1 << 3, CPU.reg.b);
}

/* 0x59: Test bit in register. */
void bit_3_c(void)
{
    bit(1 << 3, CPU.reg.c);
}

/* 0x5a: Test bit in register. */
void bit_3_d(void)
{
    bit(1 << 3, CPU.reg.d);
}

/* 0x5b: Test bit in register. */
void bit_3_e(void)
{
    bit(1 << 3, CPU.reg.e);
}

/* 0x5c: Test bit in register. */
void bit_3_h(void)
{
    bit(1 << 3, CPU.reg.h);
}

/* 0x5d: Test bit in register. */
void bit_3_l(void)
{
    bit(1 << 3, CPU.reg.l);
}

/* 0x5e: Test bit in register. */
void bit_3_hlp(void)
{
    bit(1 << 3, mmu_read_byte(CPU.reg.hl));
}

/* 0x5f: Test bit in register. */
void bit_3_a(void)
{
    bit(1 << 3, CPU.reg.a);
}

/* 0x60: Test bit in register. */
void bit_4_b(void)
{
    bit(1 << 4, CPU.reg.b);
}

/* 0x61: Test bit in register. */
void bit_4_c(void)
{
    bit(1 << 4, CPU.reg.c);
}

/* 0x62: Test bit in register. */
void bit_4_d(void)
{
    bit(1 << 4, CPU.reg.d);
}

/* 0x63: Test bit in register. */
void bit_4_e(void)
{
    bit(1 << 4, CPU.reg.e);
}

/* 0x64: Test bit in register. */
void bit_4_h(void)
{
    bit(1 << 4, CPU.reg.h);
}

/* 0x65: Test bit in register. */
void bit_4_l(void)
{
    bit(1 << 4, CPU.reg.l);
}

/* 0x66: Test bit in register. */
void bit_4_hlp(void)
{
    bit(1 << 4, mmu_read_byte(CPU.reg.hl));
}

/* 0x67: Test bit in register. */
void bit_4_a(void)
{
    bit(1 << 4, CPU.reg.a);
}

/* 0x68: Test bit in register. */
void bit_5_b(void)
{
    bit(1 << 5, CPU.reg.b);
}

/* 0x69: Test bit in register. */
void bit_5_c(void)
{
    bit(1 << 5, CPU.reg.c);
}

/* 0x6a: Test bit in register. */
void bit_5_d(void)
{
    bit(1 << 5, CPU.reg.d);
}

/* 0x6b: Test bit in register. */
void bit_5_e(void)
{
    bit(1 << 5, CPU.reg.e);
}

/* 0x6c: Test bit in register. */
void bit_5_h(void)
{
    bit(1 << 5, CPU.reg.h);
}

/* 0x6d: Test bit in register. */
void bit_5_l(void)
{
    bit(1 << 5, CPU.reg.l);
}

/* 0x6e: Test bit in register. */
void bit_5_hlp(void)
{
    bit(1 << 5, mmu_read_byte(CPU.reg.hl));
}

/* 0x6f: Test bit in register. */
void bit_5_a(void)
{
    bit(1 << 5, CPU.reg.a);
}

/* 0x70: Test bit in register. */
void bit_6_b(void)
{
    bit(1 << 6, CPU.reg.b);
}

/* 0x71: Test bit in register. */
void bit_6_c(void)
{
    bit(1 << 6, CPU.reg.c);
}

/* 0x72: Test bit in register. */
void bit_6_d(void)
{
    bit(1 << 6, CPU.reg.d);
}

/* 0x73: Test bit in register. */
void bit_6_e(void)
{
    bit(1 << 6, CPU.reg.e);
}

/* 0x74: Test bit in register. */
void bit_6_h(void)
{
    bit(1 << 6, CPU.reg.h);
}

/* 0x75: Test bit in register. */
void bit_6_l(void)
{
    bit(1 << 6, CPU.reg.l);
}

/* 0x76: Test bit in register. */
void bit_6_hlp(void)
{
    bit(1 << 6, mmu_read_byte(CPU.reg.hl));
}

/* 0x77: Test bit in register. */
void bit_6_a(void)
{
    bit(1 << 6, CPU.reg.a);
}

/* 0x78: Test bit in register. */
void bit_7_b(void)
{
    bit(1 << 7, CPU.reg.b);
}

/* 0x79: Test bit in register. */
void bit_7_c(void)
{
    bit(1 << 7, CPU.reg.c);
}

/* 0x7a: Test bit in register. */
void bit_7_d(void)
{
    bit(1 << 7, CPU.reg.d);
}

/* 0x7b: Test bit in register. */
void bit_7_e(void)
{
    bit(1 << 7, CPU.reg.e);
}

/* 0x7c: Test bit in register. */
void bit_7_h(void)
{
    bit(1 << 7, CPU.reg.h);
}

/* 0x7d: Test bit in register. */
void bit_7_l(void)
{
    bit(1 << 7, CPU.reg.l);
}

/* 0x7e: Test bit in register. */
void bit_7_hlp(void)
{
    bit(1 << 7, mmu_read_byte(CPU.reg.hl));
}

/* 0x7f: Test bit in register. */
void bit_7_a(void)
{
    bit(1 << 7, CPU.reg.a);
}

/* 0x80: Reset bit in register. */
void res_0_b(void)
{
    CPU.reg.b = res(1 << 0, CPU.reg.b);
}

/* 0x81: Reset bit in register. */
void res_0_c(void)
{
    CPU.reg.c = res(1 << 0, CPU.reg.c);
}

/* 0x82: Reset bit in register. */
void res_0_d(void)
{
    CPU.reg.d = res(1 << 0, CPU.reg.d);
}

/* 0x83: Reset bit in register. */
void res_0_e(void)
{
    CPU.reg.e = res(1 << 0, CPU.reg.e);
}

/* 0x84: Reset bit in register. */
void res_0_h(void)
{
    CPU.reg.h = res(1 << 0, CPU.reg.h);
}

/* 0x85: Reset bit in register. */
void res_0_l(void)
{
    CPU.reg.l = res(1 << 0, CPU.reg.l);
}

/* 0x86: Reset bit in register. */
void res_0_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 0, mmu_read_byte(CPU.reg.hl)));
}

/* 0x87: Reset bit in register. */
void res_0_a(void)
{
    CPU.reg.a = res(1 << 0, CPU.reg.a);
}

/* 0x88: Reset bit in register. */
void res_1_b(void)
{
    CPU.reg.b = res(1 << 1, CPU.reg.b);
}

/* 0x89: Reset bit in register. */
void res_1_c(void)
{
    CPU.reg.c = res(1 << 1, CPU.reg.c);
}

/* 0x8a: Reset bit in register. */
void res_1_d(void)
{
    CPU.reg.d = res(1 << 1, CPU.reg.d);
}

/* 0x8b: Reset bit in register. */
void res_1_e(void)
{
    CPU.reg.e = res(1 << 1, CPU.reg.e);
}

/* 0x8c: Reset bit in register. */
void res_1_h(void)
{
    CPU.reg.h = res(1 << 1, CPU.reg.h);
}

/* 0x8d: Reset bit in register. */
void res_1_l(void)
{
    CPU.reg.l = res(1 << 1, CPU.reg.l);
}

/* 0x8e: Reset bit in register. */
void res_1_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 1, mmu_read_byte(CPU.reg.hl)));
}

/* 0x8f: Reset bit in register. */
void res_1_a(void)
{
    CPU.reg.a = res(1 << 1, CPU.reg.a);
}

/* 0x90: Reset bit in register. */
void res_2_b(void)
{
    CPU.reg.b = res(1 << 2, CPU.reg.b);
}

/* 0x91: Reset bit in register. */
void res_2_c(void)
{
    CPU.reg.c = res(1 << 2, CPU.reg.c);
}

/* 0x92: Reset bit in register. */
void res_2_d(void)
{
    CPU.reg.d = res(1 << 2, CPU.reg.d);
}

/* 0x93: Reset bit in register. */
void res_2_e(void)
{
    CPU.reg.e = res(1 << 2, CPU.reg.e);
}

/* 0x94: Reset bit in register. */
void res_2_h(void)
{
    CPU.reg.h = res(1 << 2, CPU.reg.h);
}

/* 0x95: Reset bit in register. */
void res_2_l(void)
{
    CPU.reg.l = res(1 << 2, CPU.reg.l);
}

/* 0x96: Reset bit in register. */
void res_2_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 2, mmu_read_byte(CPU.reg.hl)));
}

/* 0x97: Reset bit in register. */
void res_2_a(void)
{
    CPU.reg.a = res(1 << 2, CPU.reg.a);
}

/* 0x98: Reset bit in register. */
void res_3_b(void)
{
    CPU.reg.b = res(1 << 3, CPU.reg.b);
}

/* 0x99: Reset bit in register. */
void res_3_c(void)
{
    CPU.reg.c = res(1 << 3, CPU.reg.c);
}

/* 0x9a: Reset bit in register. */
void res_3_d(void)
{
    CPU.reg.d = res(1 << 3, CPU.reg.d);
}

/* 0x9b: Reset bit in register. */
void res_3_e(void)
{
    CPU.reg.e = res(1 << 3, CPU.reg.e);
}

/* 0x9c: Reset bit in register. */
void res_3_h(void)
{
    CPU.reg.h = res(1 << 3, CPU.reg.h);
}

/* 0x9d: Reset bit in register. */
void res_3_l(void)
{
    CPU.reg.l = res(1 << 3, CPU.reg.l);
}

/* 0x9e: Reset bit in register. */
void res_3_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 3, mmu_read_byte(CPU.reg.hl)));
}

/* 0x9f: Reset bit in register. */
void res_3_a(void)
{
    CPU.reg.a = res(1 << 3, CPU.reg.a);
}

/* 0xa0: Reset bit in register. */
void res_4_b(void)
{
    CPU.reg.b = res(1 << 4, CPU.reg.b);
}

/* 0xa1: Reset bit in register. */
void res_4_c(void)
{
    CPU.reg.c = res(1 << 4, CPU.reg.c);
}

/* 0xa2: Reset bit in register. */
void res_4_d(void)
{
    CPU.reg.d = res(1 << 4, CPU.reg.d);
}

/* 0xa3: Reset bit in register. */
void res_4_e(void)
{
    CPU.reg.e = res(1 << 4, CPU.reg.e);
}

/* 0xa4: Reset bit in register. */
void res_4_h(void)
{
    CPU.reg.h = res(1 << 4, CPU.reg.h);
}

/* 0xa5: Reset bit in register. */
void res_4_l(void)
{
    CPU.reg.l = res(1 << 4, CPU.reg.l);
}

/* 0xa6: Reset bit in register. */
void res_4_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 4, mmu_read_byte(CPU.reg.hl)));
}

/* 0xa7: Reset bit in register. */
void res_4_a(void)
{
    CPU.reg.a = res(1 << 4, CPU.reg.a);
}

/* 0xa8: Reset bit in register. */
void res_5_b(void)
{
    CPU.reg.b = res(1 << 5, CPU.reg.b);
}

/* 0xa9: Reset bit in register. */
void res_5_c(void)
{
    CPU.reg.c = res(1 << 5, CPU.reg.c);
}

/* 0xaa: Reset bit in register. */
void res_5_d(void)
{
    CPU.reg.d = res(1 << 5, CPU.reg.d);
}

/* 0xab: Reset bit in register. */
void res_5_e(void)
{
    CPU.reg.e = res(1 << 5, CPU.reg.e);
}

/* 0xac: Reset bit in register. */
void res_5_h(void)
{
    CPU.reg.h = res(1 << 5, CPU.reg.h);
}

/* 0xad: Reset bit in register. */
void res_5_l(void)
{
    CPU.reg.l = res(1 << 5, CPU.reg.l);
}

/* 0xae: Reset bit in register. */
void res_5_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 5, mmu_read_byte(CPU.reg.hl)));
}

/* 0xaf: Reset bit in register. */
void res_5_a(void)
{
    CPU.reg.a = res(1 << 5, CPU.reg.a);
}

/* 0xb0: Reset bit in register. */
void res_6_b(void)
{
    CPU.reg.b = res(1 << 6, CPU.reg.b);
}

/* 0xb1: Reset bit in register. */
void res_6_c(void)
{
    CPU.reg.c = res(1 << 6, CPU.reg.c);
}

/* 0xb2: Reset bit in register. */
void res_6_d(void)
{
    CPU.reg.d = res(1 << 6, CPU.reg.d);
}

/* 0xb3: Reset bit in register. */
void res_6_e(void)
{
    CPU.reg.e = res(1 << 6, CPU.reg.e);
}

/* 0xb4: Reset bit in register. */
void res_6_h(void)
{
    CPU.reg.h = res(1 << 6, CPU.reg.h);
}

/* 0xb5: Reset bit in register. */
void res_6_l(void)
{
    CPU.reg.l = res(1 << 6, CPU.reg.l);
}

/* 0xb6: Reset bit in register. */
void res_6_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 6, mmu_read_byte(CPU.reg.hl)));
}

/* 0xb7: Reset bit in register. */
void res_6_a(void)
{
    CPU.reg.a = res(1 << 6, CPU.reg.a);
}

/* 0xb8: Reset bit in register. */
void res_7_b(void)
{
    CPU.reg.b = res(1 << 7, CPU.reg.b);
}

/* 0xb9: Reset bit in register. */
void res_7_c(void)
{
    CPU.reg.c = res(1 << 7, CPU.reg.c);
}

/* 0xba: Reset bit in register. */
void res_7_d(void)
{
    CPU.reg.d = res(1 << 7, CPU.reg.d);
}

/* 0xbb: Reset bit in register. */
void res_7_e(void)
{
    CPU.reg.e = res(1 << 7, CPU.reg.e);
}

/* 0xbc: Reset bit in register. */
void res_7_h(void)
{
    CPU.reg.h = res(1 << 7, CPU.reg.h);
}

/* 0xbd: Reset bit in register. */
void res_7_l(void)
{
    CPU.reg.l = res(1 << 7, CPU.reg.l);
}

/* 0xbe: Reset bit in register. */
void res_7_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, res(1 << 7, mmu_read_byte(CPU.reg.hl)));
}

/* 0xbf: Reset bit in register. */
void res_7_a(void)
{
    CPU.reg.a = res(1 << 7, CPU.reg.a);
}

/* 0xc0: Reset bit in register. */
void set_0_b(void)
{
    CPU.reg.b = set(1 << 0, CPU.reg.b);
}

/* 0xc1: Reset bit in register. */
void set_0_c(void)
{
    CPU.reg.c = set(1 << 0, CPU.reg.c);
}

/* 0xc2: Reset bit in register. */
void set_0_d(void)
{
    CPU.reg.d = set(1 << 0, CPU.reg.d);
}

/* 0xc3: Reset bit in register. */
void set_0_e(void)
{
    CPU.reg.e = set(1 << 0, CPU.reg.e);
}

/* 0xc4: Reset bit in register. */
void set_0_h(void)
{
    CPU.reg.h = set(1 << 0, CPU.reg.h);
}

/* 0xc5: Reset bit in register. */
void set_0_l(void)
{
    CPU.reg.l = set(1 << 0, CPU.reg.l);
}

/* 0xc6: Reset bit in register. */
void set_0_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 0, mmu_read_byte(CPU.reg.hl)));
}

/* 0xc7: Reset bit in register. */
void set_0_a(void)
{
    CPU.reg.a = set(1 << 0, CPU.reg.a);
}

/* 0xc8: Reset bit in register. */
void set_1_b(void)
{
    CPU.reg.b = set(1 << 1, CPU.reg.b);
}

/* 0xc9: Reset bit in register. */
void set_1_c(void)
{
    CPU.reg.c = set(1 << 1, CPU.reg.c);
}

/* 0xca: Reset bit in register. */
void set_1_d(void)
{
    CPU.reg.d = set(1 << 1, CPU.reg.d);
}

/* 0xcb: Reset bit in register. */
void set_1_e(void)
{
    CPU.reg.e = set(1 << 1, CPU.reg.e);
}

/* 0xcc: Reset bit in register. */
void set_1_h(void)
{
    CPU.reg.h = set(1 << 1, CPU.reg.h);
}

/* 0xcd: Reset bit in register. */
void set_1_l(void)
{
    CPU.reg.l = set(1 << 1, CPU.reg.l);
}

/* 0xce: Reset bit in register. */
void set_1_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 1, mmu_read_byte(CPU.reg.hl)));
}

/* 0xcf: Reset bit in register. */
void set_1_a(void)
{
    CPU.reg.a = set(1 << 1, CPU.reg.a);
}

/* 0xd0: Reset bit in register. */
void set_2_b(void)
{
    CPU.reg.b = set(1 << 2, CPU.reg.b);
}

/* 0xd1: Reset bit in register. */
void set_2_c(void)
{
    CPU.reg.c = set(1 << 2, CPU.reg.c);
}

/* 0xd2: Reset bit in register. */
void set_2_d(void)
{
    CPU.reg.d = set(1 << 2, CPU.reg.d);
}

/* 0xd3: Reset bit in register. */
void set_2_e(void)
{
    CPU.reg.e = set(1 << 2, CPU.reg.e);
}

/* 0xd4: Reset bit in register. */
void set_2_h(void)
{
    CPU.reg.h = set(1 << 2, CPU.reg.h);
}

/* 0xd5: Reset bit in register. */
void set_2_l(void)
{
    CPU.reg.l = set(1 << 2, CPU.reg.l);
}

/* 0xd6: Reset bit in register. */
void set_2_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 2, mmu_read_byte(CPU.reg.hl)));
}

/* 0xd7: Reset bit in register. */
void set_2_a(void)
{
    CPU.reg.a = set(1 << 2, CPU.reg.a);
}

/* 0xd8: Reset bit in register. */
void set_3_b(void)
{
    CPU.reg.b = set(1 << 3, CPU.reg.b);
}

/* 0xd9: Reset bit in register. */
void set_3_c(void)
{
    CPU.reg.c = set(1 << 3, CPU.reg.c);
}

/* 0xda: Reset bit in register. */
void set_3_d(void)
{
    CPU.reg.d = set(1 << 3, CPU.reg.d);
}

/* 0xdb: Reset bit in register. */
void set_3_e(void)
{
    CPU.reg.e = set(1 << 3, CPU.reg.e);
}

/* 0xdc: Reset bit in register. */
void set_3_h(void)
{
    CPU.reg.h = set(1 << 3, CPU.reg.h);
}

/* 0xdd: Reset bit in register. */
void set_3_l(void)
{
    CPU.reg.l = set(1 << 3, CPU.reg.l);
}

/* 0xde: Reset bit in register. */
void set_3_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 3, mmu_read_byte(CPU.reg.hl)));
}

/* 0xdf: Reset bit in register. */
void set_3_a(void)
{
    CPU.reg.a = set(1 << 3, CPU.reg.a);
}

/* 0xe0: Reset bit in register. */
void set_4_b(void)
{
    CPU.reg.b = set(1 << 4, CPU.reg.b);
}

/* 0xe1: Reset bit in register. */
void set_4_c(void)
{
    CPU.reg.c = set(1 << 4, CPU.reg.c);
}

/* 0xe2: Reset bit in register. */
void set_4_d(void)
{
    CPU.reg.d = set(1 << 4, CPU.reg.d);
}

/* 0xe3: Reset bit in register. */
void set_4_e(void)
{
    CPU.reg.e = set(1 << 4, CPU.reg.e);
}

/* 0xe4: Reset bit in register. */
void set_4_h(void)
{
    CPU.reg.h = set(1 << 4, CPU.reg.h);
}

/* 0xe5: Reset bit in register. */
void set_4_l(void)
{
    CPU.reg.l = set(1 << 4, CPU.reg.l);
}

/* 0xe6: Reset bit in register. */
void set_4_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 4, mmu_read_byte(CPU.reg.hl)));
}

/* 0xe7: Reset bit in register. */
void set_4_a(void)
{
    CPU.reg.a = set(1 << 4, CPU.reg.a);
}

/* 0xe8: Reset bit in register. */
void set_5_b(void)
{
    CPU.reg.b = set(1 << 5, CPU.reg.b);
}

/* 0xe9: Reset bit in register. */
void set_5_c(void)
{
    CPU.reg.c = set(1 << 5, CPU.reg.c);
}

/* 0xea: Reset bit in register. */
void set_5_d(void)
{
    CPU.reg.d = set(1 << 5, CPU.reg.d);
}

/* 0xeb: Reset bit in register. */
void set_5_e(void)
{
    CPU.reg.e = set(1 << 5, CPU.reg.e);
}

/* 0xec: Reset bit in register. */
void set_5_h(void)
{
    CPU.reg.h = set(1 << 5, CPU.reg.h);
}

/* 0xed: Reset bit in register. */
void set_5_l(void)
{
    CPU.reg.l = set(1 << 5, CPU.reg.l);
}

/* 0xee: Reset bit in register. */
void set_5_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 5, mmu_read_byte(CPU.reg.hl)));
}

/* 0xef: Reset bit in register. */
void set_5_a(void)
{
    CPU.reg.a = set(1 << 5, CPU.reg.a);
}

/* 0xf0: Reset bit in register. */
void set_6_b(void)
{
    CPU.reg.b = set(1 << 6, CPU.reg.b);
}

/* 0xf1: Reset bit in register. */
void set_6_c(void)
{
    CPU.reg.c = set(1 << 6, CPU.reg.c);
}

/* 0xf2: Reset bit in register. */
void set_6_d(void)
{
    CPU.reg.d = set(1 << 6, CPU.reg.d);
}

/* 0xf3: Reset bit in register. */
void set_6_e(void)
{
    CPU.reg.e = set(1 << 6, CPU.reg.e);
}

/* 0xf4: Reset bit in register. */
void set_6_h(void)
{
    CPU.reg.h = set(1 << 6, CPU.reg.h);
}

/* 0xf5: Reset bit in register. */
void set_6_l(void)
{
    CPU.reg.l = set(1 << 6, CPU.reg.l);
}

/* 0xf6: Reset bit in register. */
void set_6_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 6, mmu_read_byte(CPU.reg.hl)));
}

/* 0xf7: Reset bit in register. */
void set_6_a(void)
{
    CPU.reg.a = set(1 << 6, CPU.reg.a);
}

/* 0xf8: Reset bit in register. */
void set_7_b(void)
{
    CPU.reg.b = set(1 << 7, CPU.reg.b);
}

/* 0xf9: Reset bit in register. */
void set_7_c(void)
{
    CPU.reg.c = set(1 << 7, CPU.reg.c);
}

/* 0xfa: Reset bit in register. */
void set_7_d(void)
{
    CPU.reg.d = set(1 << 7, CPU.reg.d);
}

/* 0xfb: Reset bit in register. */
void set_7_e(void)
{
    CPU.reg.e = set(1 << 7, CPU.reg.e);
}

/* 0xfc: Reset bit in register. */
void set_7_h(void)
{
    CPU.reg.h = set(1 << 7, CPU.reg.h);
}

/* 0xfd: Reset bit in register. */
void set_7_l(void)
{
    CPU.reg.l = set(1 << 7, CPU.reg.l);
}

/* 0xfe: Reset bit in register. */
void set_7_hlp(void)
{
    mmu_write_byte(CPU.reg.hl, set(1 << 7, mmu_read_byte(CPU.reg.hl)));
}

/* 0xff: Reset bit in register. */
void set_7_a(void)
{
    CPU.reg.a = set(1 << 7, CPU.reg.a);
}
