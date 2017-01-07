#include <stdio.h>
#include "cpu_ext_ops.h"
#include "cpu.h"
#include "cpu_utils.h"
#include "mmu.h"

extern cpu_t g_cpu;

typedef struct {
    const char *asm;
    void (*execute)(void);
} ext_instruction_t;

const ext_instruction_t g_ext_instr[256] = {
    {"RLC B", rlc_b},            // 0x00
    {"RLC C", rlc_c},            // 0x01
    {"RLC D", rlc_d},            // 0x02
    {"RLC E", rlc_e},            // 0x03
    {"RLC H", rlc_h},            // 0x04
    {"RLC L", rlc_l},            // 0x05
    {"RLC (HL)", rlc_hlp},       // 0x06
    {"RLC A", rlc_a},            // 0x07
    {"RRC B", rrc_b},            // 0x08
    {"RRC C", rrc_c},            // 0x09
    {"RRC D", rrc_d},            // 0x0a
    {"RRC E", rrc_e},            // 0x0b
    {"RRC H", rrc_h},            // 0x0c
    {"RRC L", rrc_l},            // 0x0d
    {"RRC (HL)", rrc_hlp},       // 0x0e
    {"RRC A", rrc_a},            // 0x0f
    {"RL B", rl_b},              // 0x10
    {"RL C", rl_c},              // 0x11
    {"RL D", rl_d},              // 0x12
    {"RL E", rl_e},              // 0x13
    {"RL H", rl_h},              // 0x14
    {"RL L", rl_l},              // 0x15
    {"RL (HL)", rl_hlp},         // 0x16
    {"RL A", rl_a},              // 0x17
    {"RR B", rr_b},              // 0x18
    {"RR C", rr_c},              // 0x19
    {"RR D", rr_d},              // 0x1a
    {"RR E", rr_e},              // 0x1b
    {"RR H", rr_h},              // 0x1c
    {"RR L", rr_l},              // 0x1d
    {"RR (HL)", rr_hlp},         // 0x1e
    {"RR A", rr_a},              // 0x1f
    {"SLA B", sla_b},            // 0x20
    {"SLA C", sla_c},            // 0x21
    {"SLA D", sla_d},            // 0x22
    {"SLA E", sla_e},            // 0x23
    {"SLA H", sla_h},            // 0x24
    {"SLA L", sla_l},            // 0x25
    {"SLA (HL)", sla_hlp},       // 0x26
    {"SLA A", sla_a},            // 0x27
    {"SRA B", sra_b},            // 0x28
    {"SRA C", sra_c},            // 0x29
    {"SRA D", sra_d},            // 0x2a
    {"SRA E", sra_e},            // 0x2b
    {"SRA H", sra_h},            // 0x2c
    {"SRA L", sra_l},            // 0x2d
    {"SRA (HL)", sra_hlp},       // 0x2e
    {"SRA A", sra_a},            // 0x2f
    {"SWAP B", swap_b},          // 0x30
    {"SWAP C", swap_c},          // 0x31
    {"SWAP D", swap_d},          // 0x32
    {"SWAP E", swap_e},          // 0x33
    {"SWAP H", swap_h},          // 0x34
    {"SWAP L", swap_l},          // 0x35
    {"SWAP (HL)", swap_hlp},     // 0x36
    {"SWAP A", swap_a},          // 0x37
    {"SRL B", srl_b},            // 0x38
    {"SRL C", srl_c},            // 0x39
    {"SRL D", srl_d},            // 0x3a
    {"SRL E", srl_e},            // 0x3b
    {"SRL H", srl_h},            // 0x3c
    {"SRL L", srl_l},            // 0x3d
    {"SRL (HL)", srl_hlp},       // 0x3e
    {"SRL A", srl_a},            // 0x3f
    {"BIT 0, B", bit_0_b},       // 0x40
    {"BIT 0, C", bit_0_c},       // 0x41
    {"BIT 0, D", bit_0_d},       // 0x42
    {"BIT 0, E", bit_0_e},       // 0x43
    {"BIT 0, H", bit_0_h},       // 0x44
    {"BIT 0, L", bit_0_l},       // 0x45
    {"BIT 0, (HL)", bit_0_hlp},  // 0x46
    {"BIT 0, A", bit_0_a},       // 0x47
    {"BIT 1, B", bit_1_b},       // 0x48
    {"BIT 1, C", bit_1_c},       // 0x49
    {"BIT 1, D", bit_1_d},       // 0x4a
    {"BIT 1, E", bit_1_e},       // 0x4b
    {"BIT 1, H", bit_1_h},       // 0x4c
    {"BIT 1, L", bit_1_l},       // 0x4d
    {"BIT 1, (HL)", bit_1_hlp},  // 0x4e
    {"BIT 1, A", bit_1_a},       // 0x4f
    {"BIT 2, B", bit_2_b},       // 0x50
    {"BIT 2, C", bit_2_c},       // 0x51
    {"BIT 2, D", bit_2_d},       // 0x52
    {"BIT 2, E", bit_2_e},       // 0x53
    {"BIT 2, H", bit_2_h},       // 0x54
    {"BIT 2, L", bit_2_l},       // 0x55
    {"BIT 2, (HL)", bit_2_hlp},  // 0x56
    {"BIT 2, A", bit_2_a},       // 0x57
    {"BIT 3, B", bit_3_b},       // 0x58
    {"BIT 3, C", bit_3_c},       // 0x59
    {"BIT 3, D", bit_3_d},       // 0x5a
    {"BIT 3, E", bit_3_e},       // 0x5b
    {"BIT 3, H", bit_3_h},       // 0x5c
    {"BIT 3, L", bit_3_l},       // 0x5d
    {"BIT 3, (HL)", bit_3_hlp},  // 0x5e
    {"BIT 3, A", bit_3_a},       // 0x5f
    {"BIT 4, B", bit_4_b},       // 0x60
    {"BIT 4, C", bit_4_c},       // 0x61
    {"BIT 4, D", bit_4_d},       // 0x62
    {"BIT 4, E", bit_4_e},       // 0x63
    {"BIT 4, H", bit_4_h},       // 0x64
    {"BIT 4, L", bit_4_l},       // 0x65
    {"BIT 4, (HL)", bit_4_hlp},  // 0x66
    {"BIT 4, A", bit_4_a},       // 0x67
    {"BIT 5, B", bit_5_b},       // 0x68
    {"BIT 5, C", bit_5_c},       // 0x69
    {"BIT 5, D", bit_5_d},       // 0x6a
    {"BIT 5, E", bit_5_e},       // 0x6b
    {"BIT 6, H", bit_5_h},       // 0x6c
    {"BIT 6, L", bit_5_l},       // 0x6d
    {"BIT 5, (HL)", bit_5_hlp},  // 0x6e
    {"BIT 5, A", bit_5_a},       // 0x6f
    {"BIT 6, B", bit_6_b},       // 0x70
    {"BIT 6, C", bit_6_c},       // 0x71
    {"BIT 6, D", bit_6_d},       // 0x72
    {"BIT 6, E", bit_6_e},       // 0x73
    {"BIT 6, H", bit_6_h},       // 0x74
    {"BIT 6, L", bit_6_l},       // 0x75
    {"BIT 6, (HL)", bit_6_hlp},  // 0x76
    {"BIT 6, A", bit_6_a},       // 0x77
    {"BIT 7, B", bit_7_b},       // 0x78
    {"BIT 7, C", bit_7_c},       // 0x79
    {"BIT 7, D", bit_7_d},       // 0x7a
    {"BIT 7, E", bit_7_e},       // 0x7b
    {"BIT 7, H", bit_7_h},       // 0x7c
    {"BIT 7, L", bit_7_l},       // 0x7d
    {"BIT 7, (HL)", bit_7_hlp},  // 0x7e
    {"BIT 7, A", bit_7_a},       // 0x7f
    {"RES 0, B", res_0_b},       // 0x80
    {"RES 0, C", res_0_c},       // 0x81
    {"RES 0, D", res_0_d},       // 0x82
    {"RES 0, E", res_0_e},       // 0x83
    {"RES 0, H", res_0_h},       // 0x84
    {"RES 0, L", res_0_l},       // 0x85
    {"RES 0, (HL)", res_0_hlp},  // 0x86
    {"RES 0, A", res_0_a},       // 0x87
    {"RES 1, B", res_1_b},       // 0x88
    {"RES 1, C", res_1_c},       // 0x89
    {"RES 1, D", res_1_d},       // 0x8a
    {"RES 1, E", res_1_e},       // 0x8b
    {"RES 1, H", res_1_h},       // 0x8c
    {"RES 1, L", res_1_l},       // 0x8d
    {"RES 1, (HL)", res_1_hlp},  // 0x8e
    {"RES 1, A", res_1_a},       // 0x8f
    {"RES 2, B", res_2_b},       // 0x90
    {"RES 2, C", res_2_c},       // 0x91
    {"RES 2, D", res_2_d},       // 0x92
    {"RES 2, E", res_2_e},       // 0x93
    {"RES 2, H", res_2_h},       // 0x94
    {"RES 2, L", res_2_l},       // 0x95
    {"RES 2, (HL)", res_2_hlp},  // 0x96
    {"RES 2, A", res_2_a},       // 0x97
    {"RES 3, B", res_3_b},       // 0x98
    {"RES 3, C", res_3_c},       // 0x99
    {"RES 3, D", res_3_d},       // 0x9a
    {"RES 3, E", res_3_e},       // 0x9b
    {"RES 3, H", res_3_h},       // 0x9c
    {"RES 3, L", res_3_l},       // 0x9d
    {"RES 3, (HL)", res_3_hlp},  // 0x9e
    {"RES 3, A", res_3_a},       // 0x9f
    {"RES 4, B", res_4_b},       // 0xa0
    {"RES 4, C", res_4_c},       // 0xa1
    {"RES 4, D", res_4_d},       // 0xa2
    {"RES 4, E", res_4_e},       // 0xa3
    {"RES 4, H", res_4_h},       // 0xa4
    {"RES 4, L", res_4_l},       // 0xa5
    {"RES 4, (HL)", res_4_hlp},  // 0xa6
    {"RES 4, A", res_4_a},       // 0xa7
    {"RES 5, B", res_5_b},       // 0xa8
    {"RES 5, C", res_5_c},       // 0xa9
    {"RES 5, D", res_5_d},       // 0xaa
    {"RES 5, E", res_5_e},       // 0xab
    {"RES 5, H", res_5_h},       // 0xac
    {"RES 5, L", res_5_l},       // 0xad
    {"RES 5, (HL)", res_5_hlp},  // 0xae
    {"RES 5, A", res_5_a},       // 0xaf
    {"RES 6, B", res_6_b},       // 0xb0
    {"RES 6, C", res_6_c},       // 0xb1
    {"RES 6, D", res_6_d},       // 0xb2
    {"RES 6, E", res_6_e},       // 0xb3
    {"RES 6, H", res_6_h},       // 0xb4
    {"RES 6, L", res_6_l},       // 0xb5
    {"RES 6, (HL)", res_6_hlp},  // 0xb6
    {"RES 6, A", res_6_a},       // 0xb7
    {"RES 7, B", res_7_b},       // 0xb8
    {"RES 7, C", res_7_c},       // 0xb9
    {"RES 7, D", res_7_d},       // 0xba
    {"RES 7, E", res_7_e},       // 0xbb
    {"RES 7, H", res_7_h},       // 0xbc
    {"RES 7, L", res_7_l},       // 0xbd
    {"RES 7, (HL)", res_7_hlp},  // 0xbe
    {"RES 7, A", res_7_a},       // 0xbf
    {"SET 0, B", set_0_b},       // 0xc0
    {"SET 0, C", set_0_c},       // 0xc1
    {"SET 0, D", set_0_d},       // 0xc2
    {"SET 0, E", set_0_e},       // 0xc3
    {"SET 0, H", set_0_h},       // 0xc4
    {"SET 0, L", set_0_l},       // 0xc5
    {"SET 0, (HL)", set_0_hlp},  // 0xc6
    {"SET 0, A", set_0_a},       // 0xc7
    {"SET 1, B", set_1_b},       // 0xc8
    {"SET 1, C", set_1_c},       // 0xc9
    {"SET 1, D", set_1_d},       // 0xca
    {"SET 1, E", set_1_e},       // 0xcb
    {"SET 1, H", set_1_h},       // 0xcc
    {"SET 1, L", set_1_l},       // 0xcd
    {"SET 1, (HL)", set_1_hlp},  // 0xce
    {"SET 1, A", set_1_a},       // 0xcf
    {"SET 2, B", set_2_b},       // 0xd0
    {"SET 2, C", set_2_c},       // 0xd1
    {"SET 2, D", set_2_d},       // 0xd2
    {"SET 2, E", set_2_e},       // 0xd3
    {"SET 2, H", set_2_h},       // 0xd4
    {"SET 2, L", set_2_l},       // 0xd5
    {"SET 2, (HL)", set_2_hlp},  // 0xd6
    {"SET 2, A", set_2_a},       // 0xd7
    {"SET 3, B", set_3_b},       // 0xd8
    {"SET 3, C", set_3_c},       // 0xd9
    {"SET 3, D", set_3_d},       // 0xda
    {"SET 3, E", set_3_e},       // 0xdb
    {"SET 3, H", set_3_h},       // 0xdc
    {"SET 3, L", set_3_l},       // 0xdd
    {"SET 3, (HL)", set_3_hlp},  // 0xde
    {"SET 3, A", set_3_a},       // 0xdf
    {"SET 4, B", set_4_b},       // 0xe0
    {"SET 4, C", set_4_c},       // 0xe1
    {"SET 4, D", set_4_d},       // 0xe2
    {"SET 4, E", set_4_e},       // 0xe3
    {"SET 4, H", set_4_h},       // 0xe4
    {"SET 4, L", set_4_l},       // 0xe5
    {"SET 4, (HL)", set_4_hlp},  // 0xe6
    {"SET 4, A", set_4_a},       // 0xe7
    {"SET 5, B", set_5_b},       // 0xe8
    {"SET 5, C", set_5_c},       // 0xe9
    {"SET 5, D", set_5_d},       // 0xea
    {"SET 5, E", set_5_e},       // 0xeb
    {"SET 5, H", set_5_h},       // 0xec
    {"SET 5, L", set_5_l},       // 0xed
    {"SET 5, (HL)", set_5_hlp},  // 0xee
    {"SET 5, A", set_5_a},       // 0xef
    {"SET 6, B", set_6_b},       // 0xf0
    {"SET 6, C", set_6_c},       // 0xf1
    {"SET 6, D", set_6_d},       // 0xf2
    {"SET 6, E", set_6_e},       // 0xf3
    {"SET 6, H", set_6_h},       // 0xf4
    {"SET 6, L", set_6_l},       // 0xf5
    {"SET 6, (HL)", set_6_hlp},  // 0xf6
    {"SET 6, A", set_6_a},       // 0xf7
    {"SET 7, B", set_7_b},       // 0xf8
    {"SET 7, C", set_7_c},       // 0xf9
    {"SET 7, D", set_7_d},       // 0xfa
    {"SET 7, E", set_7_e},       // 0xfb
    {"SET 7, H", set_7_h},       // 0xfc
    {"SET 7, L", set_7_l},       // 0xfd
    {"SET 7, (HL)", set_7_hlp},  // 0xfe
    {"SET 7, A", set_7_a},       // 0xff
};

/**
 * Gameboy CPU (LR35902) CB instruction cycles divided by 4.
 * Values based in the table found in:
 * http://pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */
const uint8_t ext_instr_ticks[256] = {
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x0_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x1_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x2_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x3_
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 0x4_
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 0x5_
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 0x6_
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,  // 0x7_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x8_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0x9_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xa_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xb_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xc_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xd_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xe_
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,  // 0xf_
};

void print_ext_ops(char *str, uint8_t opcode)
{
    sprintf(str, "0xcb%02x: %s", opcode, g_ext_instr[opcode].asm);
}

/* 0xcb: Extended operations. */
void cb_n(uint8_t opcode)
{
    g_ext_instr[opcode].execute();
    g_cpu.clock.step += ext_instr_ticks[opcode];
}

/* 0x00: Rotate B with carry. */
void rlc_b(void)
{
    g_cpu.reg.b = rlc(g_cpu.reg.b);
}

/* 0x01: Rotate C with carry. */
void rlc_c(void)
{
    g_cpu.reg.c = rlc(g_cpu.reg.c);
}

/* 0x02: Rotate D with carry. */
void rlc_d(void)
{
    g_cpu.reg.d = rlc(g_cpu.reg.d);
}

/* 0x03: Rotate E with carry. */
void rlc_e(void)
{
    g_cpu.reg.e = rlc(g_cpu.reg.e);
}

/* 0x04: Rotate H with carry. */
void rlc_h(void)
{
    g_cpu.reg.h = rlc(g_cpu.reg.h);
}

/* 0x05: Rotate L with carry. */
void rlc_l(void)
{
    g_cpu.reg.l = rlc(g_cpu.reg.l);
}

/* 0x06: Rotate (HL) with carry. */
void rlc_hlp(void)
{
    uint8_t val = rlc(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x07: Rotate A with carry. */
void rlc_a(void)
{
    g_cpu.reg.a = rlc(g_cpu.reg.a);
}

/* 0x08: Rotate B with carry. */
void rrc_b(void)
{
    g_cpu.reg.b = rrc(g_cpu.reg.b);
}

/* 0x09: Rotate C with carry. */
void rrc_c(void)
{
    g_cpu.reg.c = rrc(g_cpu.reg.c);
}

/* 0x0a: Rotate D with carry. */
void rrc_d(void)
{
    g_cpu.reg.d = rrc(g_cpu.reg.d);
}

/* 0x0b: Rotate E with carry. */
void rrc_e(void)
{
    g_cpu.reg.e = rrc(g_cpu.reg.e);
}

/* 0x0c: Rotate H with carry. */
void rrc_h(void)
{
    g_cpu.reg.h = rrc(g_cpu.reg.h);
}

/* 0x0d: Rotate L with carry. */
void rrc_l(void)
{
    g_cpu.reg.l = rrc(g_cpu.reg.l);
}

/* 0x0e: Rotate (HL) with carry. */
void rrc_hlp(void)
{
    uint8_t val = rrc(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x0f: Rotate A with carry. */
void rrc_a(void)
{
    g_cpu.reg.a = rrc(g_cpu.reg.a);
}

/* 0x10: Rotate B left through Carry flag. */
void rl_b(void)
{
    g_cpu.reg.b = rl(g_cpu.reg.b);
}

/* 0x11: Rotate C left through Carry flag. */
void rl_c(void)
{
    g_cpu.reg.c = rl(g_cpu.reg.c);
}

/* 0x12: Rotate D left through Carry flag. */
void rl_d(void)
{
    g_cpu.reg.d = rl(g_cpu.reg.d);
}

/* 0x13: Rotate E left through Carry flag. */
void rl_e(void)
{
    g_cpu.reg.e = rl(g_cpu.reg.e);
}

/* 0x14: Rotate H left through Carry flag. */
void rl_h(void)
{
    g_cpu.reg.h = rl(g_cpu.reg.h);
}

/* 0x15: Rotate L left through Carry flag. */
void rl_l(void)
{
    g_cpu.reg.l = rl(g_cpu.reg.l);
}

/* 0x16: Rotate (HL) with carry. */
void rl_hlp(void)
{
    uint8_t val = rl(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x17: Rotate A left through Carry flag. */
void rl_a(void)
{
    g_cpu.reg.a = rl(g_cpu.reg.a);
}

/* 0x18: Rotate B right through carry flag. */
void rr_b(void)
{
    g_cpu.reg.b = rr(g_cpu.reg.b);
}

/* 0x19: Rotate C right through carry flag. */
void rr_c(void)
{
    g_cpu.reg.c = rr(g_cpu.reg.c);
}

/* 0x1a: Rotate D right through carry flag. */
void rr_d(void)
{
    g_cpu.reg.d = rr(g_cpu.reg.d);
}

/* 0x1b: Rotate E right through carry flag. */
void rr_e(void)
{
    g_cpu.reg.e = rr(g_cpu.reg.e);
}

/* 0x1c: Rotate H right through carry flag. */
void rr_h(void)
{
    g_cpu.reg.h = rr(g_cpu.reg.h);
}

/* 0x1d: Rotate L right through carry flag. */
void rr_l(void)
{
    g_cpu.reg.l = rr(g_cpu.reg.l);
}

/* 0x1e: Rotate (HL) right through carry flag. */
void rr_hlp(void)
{
    uint8_t val = rr(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x1f: Rotate A right through carry flag. */
void rr_a(void)
{
    g_cpu.reg.a = rr(g_cpu.reg.a);
}

/* 0x20: Shift B left into Carry flag. */
void sla_b(void)
{
    g_cpu.reg.b = sla(g_cpu.reg.b);
}

/* 0x21: Shift C left into Carry flag. */
void sla_c(void)
{
    g_cpu.reg.c = sla(g_cpu.reg.c);
}

/* 0x22: Shift D left into Carry flag. */
void sla_d(void)
{
    g_cpu.reg.d = sla(g_cpu.reg.d);
}

/* 0x23: Shift E left into Carry flag. */
void sla_e(void)
{
    g_cpu.reg.e = sla(g_cpu.reg.e);
}

/* 0x24: Shift H left into Carry flag. */
void sla_h(void)
{
    g_cpu.reg.h = sla(g_cpu.reg.h);
}

/* 0x25: Shift L left into Carry flag. */
void sla_l(void)
{
    g_cpu.reg.l = sla(g_cpu.reg.l);
}

/* 0x26: Shift (HL) with carry. */
void sla_hlp(void)
{
    uint8_t val = sla(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x27: Shift A left into Carry flag. */
void sla_a(void)
{
    g_cpu.reg.a = sla(g_cpu.reg.a);
}

/* 0x28: Shift B right into Carry flag. */
void sra_b(void)
{
    g_cpu.reg.b = sra(g_cpu.reg.b);
}

/* 0x29: Shift C right into Carry flag. */
void sra_c(void)
{
    g_cpu.reg.c = sra(g_cpu.reg.c);
}

/* 0x2a: Shift D right into Carry flag. */
void sra_d(void)
{
    g_cpu.reg.d = sra(g_cpu.reg.d);
}

/* 0x2b: Shift E right into Carry flag. */
void sra_e(void)
{
    g_cpu.reg.e = sra(g_cpu.reg.e);
}

/* 0x2c: Shift H right into Carry flag. */
void sra_h(void)
{
    g_cpu.reg.h = sra(g_cpu.reg.h);
}

/* 0x2d: Shift L right into Carry flag. */
void sra_l(void)
{
    g_cpu.reg.l = sra(g_cpu.reg.l);
}

/* 0x2e: Shift (HL) right into Carry flag. */
void sra_hlp(void)
{
    uint8_t val = sra(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x2f: Shift A right into Carry flag. */
void sra_a(void)
{
    g_cpu.reg.a = sra(g_cpu.reg.a);
}

/* 0x30: Swap upper & lower nibbles of n. */
void swap_b(void)
{
    g_cpu.reg.b = swap(g_cpu.reg.b);
}

/* 0x31: Swap upper & lower nibbles of n. */
void swap_c(void)
{
    g_cpu.reg.c = swap(g_cpu.reg.c);
}

/* 0x32: Swap upper & lower nibbles of n. */
void swap_d(void)
{
    g_cpu.reg.d = swap(g_cpu.reg.d);
}

/* 0x33: Swap upper & lower nibbles of n. */
void swap_e(void)
{
    g_cpu.reg.e = swap(g_cpu.reg.e);
}

/* 0x34: Swap upper & lower nibbles of n. */
void swap_h(void)
{
    g_cpu.reg.h = swap(g_cpu.reg.h);
}

/* 0x35: Swap upper & lower nibbles of n. */
void swap_l(void)
{
    g_cpu.reg.l = swap(g_cpu.reg.l);
}

/* 0x36: Swap upper & lower nibbles of n. */
void swap_hlp(void)
{
    uint8_t val = swap(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x37: Swap upper & lower nibbles of n. */
void swap_a(void)
{
    g_cpu.reg.a = swap(g_cpu.reg.a);
}

/* 0x38: Shift B right into Carry flag. */
void srl_b(void)
{
    g_cpu.reg.b = srl(g_cpu.reg.b);
}

/* 0x39: Shift C right into Carry flag. */
void srl_c(void)
{
    g_cpu.reg.c = srl(g_cpu.reg.c);
}

/* 0x3a: Shift D right into Carry flag. */
void srl_d(void)
{
    g_cpu.reg.d = srl(g_cpu.reg.d);
}

/* 0x3b: Shift E right into Carry flag. */
void srl_e(void)
{
    g_cpu.reg.e = srl(g_cpu.reg.e);
}

/* 0x3c: Shift H right into Carry flag. */
void srl_h(void)
{
    g_cpu.reg.h = srl(g_cpu.reg.h);
}

/* 0x3d: Shift L right into Carry flag. */
void srl_l(void)
{
    g_cpu.reg.l = srl(g_cpu.reg.l);
}

/* 0x3e: Shift (HL) right into Carry flag. */
void srl_hlp(void)
{
    uint8_t val = srl(mmu_read_byte(g_cpu.reg.hl));
    mmu_write_byte(g_cpu.reg.hl, val);
}

/* 0x3f: Shift A right into Carry flag. */
void srl_a(void)
{
    g_cpu.reg.a = srl(g_cpu.reg.a);
}

/* 0x40: Test bit in register. */
void bit_0_b(void)
{
    bit(1 << 0, g_cpu.reg.b);
}

/* 0x41: Test bit in register. */
void bit_0_c(void)
{
    bit(1 << 0, g_cpu.reg.c);
}

/* 0x42: Test bit in register. */
void bit_0_d(void)
{
    bit(1 << 0, g_cpu.reg.d);
}

/* 0x43: Test bit in register. */
void bit_0_e(void)
{
    bit(1 << 0, g_cpu.reg.e);
}

/* 0x44: Test bit in register. */
void bit_0_h(void)
{
    bit(1 << 0, g_cpu.reg.h);
}

/* 0x45: Test bit in register. */
void bit_0_l(void)
{
    bit(1 << 0, g_cpu.reg.l);
}

/* 0x46: Test bit in register. */
void bit_0_hlp(void)
{
    bit(1 << 0, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x47: Test bit in register. */
void bit_0_a(void)
{
    bit(1 << 0, g_cpu.reg.a);
}

/* 0x48: Test bit in register. */
void bit_1_b(void)
{
    bit(1 << 1, g_cpu.reg.b);
}

/* 0x49: Test bit in register. */
void bit_1_c(void)
{
    bit(1 << 1, g_cpu.reg.c);
}

/* 0x4a: Test bit in register. */
void bit_1_d(void)
{
    bit(1 << 1, g_cpu.reg.d);
}

/* 0x4b: Test bit in register. */
void bit_1_e(void)
{
    bit(1 << 1, g_cpu.reg.e);
}

/* 0x4c: Test bit in register. */
void bit_1_h(void)
{
    bit(1 << 1, g_cpu.reg.h);
}

/* 0x4d: Test bit in register. */
void bit_1_l(void)
{
    bit(1 << 1, g_cpu.reg.l);
}

/* 0x4e: Test bit in register. */
void bit_1_hlp(void)
{
    bit(1 << 1, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x4f: Test bit in register. */
void bit_1_a(void)
{
    bit(1 << 1, g_cpu.reg.a);
}

/* 0x50: Test bit in register. */
void bit_2_b(void)
{
    bit(1 << 2, g_cpu.reg.b);
}

/* 0x51: Test bit in register. */
void bit_2_c(void)
{
    bit(1 << 2, g_cpu.reg.c);
}

/* 0x52: Test bit in register. */
void bit_2_d(void)
{
    bit(1 << 2, g_cpu.reg.d);
}

/* 0x53: Test bit in register. */
void bit_2_e(void)
{
    bit(1 << 2, g_cpu.reg.e);
}

/* 0x54: Test bit in register. */
void bit_2_h(void)
{
    bit(1 << 2, g_cpu.reg.h);
}

/* 0x55: Test bit in register. */
void bit_2_l(void)
{
    bit(1 << 2, g_cpu.reg.l);
}

/* 0x56: Test bit in register. */
void bit_2_hlp(void)
{
    bit(1 << 2, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x57: Test bit in register. */
void bit_2_a(void)
{
    bit(1 << 2, g_cpu.reg.a);
}

/* 0x58: Test bit in register. */
void bit_3_b(void)
{
    bit(1 << 3, g_cpu.reg.b);
}

/* 0x59: Test bit in register. */
void bit_3_c(void)
{
    bit(1 << 3, g_cpu.reg.c);
}

/* 0x5a: Test bit in register. */
void bit_3_d(void)
{
    bit(1 << 3, g_cpu.reg.d);
}

/* 0x5b: Test bit in register. */
void bit_3_e(void)
{
    bit(1 << 3, g_cpu.reg.e);
}

/* 0x5c: Test bit in register. */
void bit_3_h(void)
{
    bit(1 << 3, g_cpu.reg.h);
}

/* 0x5d: Test bit in register. */
void bit_3_l(void)
{
    bit(1 << 3, g_cpu.reg.l);
}

/* 0x5e: Test bit in register. */
void bit_3_hlp(void)
{
    bit(1 << 3, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x5f: Test bit in register. */
void bit_3_a(void)
{
    bit(1 << 3, g_cpu.reg.a);
}

/* 0x60: Test bit in register. */
void bit_4_b(void)
{
    bit(1 << 4, g_cpu.reg.b);
}

/* 0x61: Test bit in register. */
void bit_4_c(void)
{
    bit(1 << 4, g_cpu.reg.c);
}

/* 0x62: Test bit in register. */
void bit_4_d(void)
{
    bit(1 << 4, g_cpu.reg.d);
}

/* 0x63: Test bit in register. */
void bit_4_e(void)
{
    bit(1 << 4, g_cpu.reg.e);
}

/* 0x64: Test bit in register. */
void bit_4_h(void)
{
    bit(1 << 4, g_cpu.reg.h);
}

/* 0x65: Test bit in register. */
void bit_4_l(void)
{
    bit(1 << 4, g_cpu.reg.l);
}

/* 0x66: Test bit in register. */
void bit_4_hlp(void)
{
    bit(1 << 4, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x67: Test bit in register. */
void bit_4_a(void)
{
    bit(1 << 4, g_cpu.reg.a);
}

/* 0x68: Test bit in register. */
void bit_5_b(void)
{
    bit(1 << 5, g_cpu.reg.b);
}

/* 0x69: Test bit in register. */
void bit_5_c(void)
{
    bit(1 << 5, g_cpu.reg.c);
}

/* 0x6a: Test bit in register. */
void bit_5_d(void)
{
    bit(1 << 5, g_cpu.reg.d);
}

/* 0x6b: Test bit in register. */
void bit_5_e(void)
{
    bit(1 << 5, g_cpu.reg.e);
}

/* 0x6c: Test bit in register. */
void bit_5_h(void)
{
    bit(1 << 5, g_cpu.reg.h);
}

/* 0x6d: Test bit in register. */
void bit_5_l(void)
{
    bit(1 << 5, g_cpu.reg.l);
}

/* 0x6e: Test bit in register. */
void bit_5_hlp(void)
{
    bit(1 << 5, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x6f: Test bit in register. */
void bit_5_a(void)
{
    bit(1 << 5, g_cpu.reg.a);
}

/* 0x70: Test bit in register. */
void bit_6_b(void)
{
    bit(1 << 6, g_cpu.reg.b);
}

/* 0x71: Test bit in register. */
void bit_6_c(void)
{
    bit(1 << 6, g_cpu.reg.c);
}

/* 0x72: Test bit in register. */
void bit_6_d(void)
{
    bit(1 << 6, g_cpu.reg.d);
}

/* 0x73: Test bit in register. */
void bit_6_e(void)
{
    bit(1 << 6, g_cpu.reg.e);
}

/* 0x74: Test bit in register. */
void bit_6_h(void)
{
    bit(1 << 6, g_cpu.reg.h);
}

/* 0x75: Test bit in register. */
void bit_6_l(void)
{
    bit(1 << 6, g_cpu.reg.l);
}

/* 0x76: Test bit in register. */
void bit_6_hlp(void)
{
    bit(1 << 6, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x77: Test bit in register. */
void bit_6_a(void)
{
    bit(1 << 6, g_cpu.reg.a);
}

/* 0x78: Test bit in register. */
void bit_7_b(void)
{
    bit(1 << 7, g_cpu.reg.b);
}

/* 0x79: Test bit in register. */
void bit_7_c(void)
{
    bit(1 << 7, g_cpu.reg.c);
}

/* 0x7a: Test bit in register. */
void bit_7_d(void)
{
    bit(1 << 7, g_cpu.reg.d);
}

/* 0x7b: Test bit in register. */
void bit_7_e(void)
{
    bit(1 << 7, g_cpu.reg.e);
}

/* 0x7c: Test bit in register. */
void bit_7_h(void)
{
    bit(1 << 7, g_cpu.reg.h);
}

/* 0x7d: Test bit in register. */
void bit_7_l(void)
{
    bit(1 << 7, g_cpu.reg.l);
}

/* 0x7e: Test bit in register. */
void bit_7_hlp(void)
{
    bit(1 << 7, mmu_read_byte(g_cpu.reg.hl));
}

/* 0x7f: Test bit in register. */
void bit_7_a(void)
{
    bit(1 << 7, g_cpu.reg.a);
}

/* 0x80: Reset bit in register. */
void res_0_b(void)
{
    g_cpu.reg.b = res(1 << 0, g_cpu.reg.b);
}

/* 0x81: Reset bit in register. */
void res_0_c(void)
{
    g_cpu.reg.c = res(1 << 0, g_cpu.reg.c);
}

/* 0x82: Reset bit in register. */
void res_0_d(void)
{
    g_cpu.reg.d = res(1 << 0, g_cpu.reg.d);
}

/* 0x83: Reset bit in register. */
void res_0_e(void)
{
    g_cpu.reg.e = res(1 << 0, g_cpu.reg.e);
}

/* 0x84: Reset bit in register. */
void res_0_h(void)
{
    g_cpu.reg.h = res(1 << 0, g_cpu.reg.h);
}

/* 0x85: Reset bit in register. */
void res_0_l(void)
{
    g_cpu.reg.l = res(1 << 0, g_cpu.reg.l);
}

/* 0x86: Reset bit in register. */
void res_0_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 0, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0x87: Reset bit in register. */
void res_0_a(void)
{
    g_cpu.reg.a = res(1 << 0, g_cpu.reg.a);
}

/* 0x88: Reset bit in register. */
void res_1_b(void)
{
    g_cpu.reg.b = res(1 << 1, g_cpu.reg.b);
}

/* 0x89: Reset bit in register. */
void res_1_c(void)
{
    g_cpu.reg.c = res(1 << 1, g_cpu.reg.c);
}

/* 0x8a: Reset bit in register. */
void res_1_d(void)
{
    g_cpu.reg.d = res(1 << 1, g_cpu.reg.d);
}

/* 0x8b: Reset bit in register. */
void res_1_e(void)
{
    g_cpu.reg.e = res(1 << 1, g_cpu.reg.e);
}

/* 0x8c: Reset bit in register. */
void res_1_h(void)
{
    g_cpu.reg.h = res(1 << 1, g_cpu.reg.h);
}

/* 0x8d: Reset bit in register. */
void res_1_l(void)
{
    g_cpu.reg.l = res(1 << 1, g_cpu.reg.l);
}

/* 0x8e: Reset bit in register. */
void res_1_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 1, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0x8f: Reset bit in register. */
void res_1_a(void)
{
    g_cpu.reg.a = res(1 << 1, g_cpu.reg.a);
}

/* 0x90: Reset bit in register. */
void res_2_b(void)
{
    g_cpu.reg.b = res(1 << 2, g_cpu.reg.b);
}

/* 0x91: Reset bit in register. */
void res_2_c(void)
{
    g_cpu.reg.c = res(1 << 2, g_cpu.reg.c);
}

/* 0x92: Reset bit in register. */
void res_2_d(void)
{
    g_cpu.reg.d = res(1 << 2, g_cpu.reg.d);
}

/* 0x93: Reset bit in register. */
void res_2_e(void)
{
    g_cpu.reg.e = res(1 << 2, g_cpu.reg.e);
}

/* 0x94: Reset bit in register. */
void res_2_h(void)
{
    g_cpu.reg.h = res(1 << 2, g_cpu.reg.h);
}

/* 0x95: Reset bit in register. */
void res_2_l(void)
{
    g_cpu.reg.l = res(1 << 2, g_cpu.reg.l);
}

/* 0x96: Reset bit in register. */
void res_2_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 2, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0x97: Reset bit in register. */
void res_2_a(void)
{
    g_cpu.reg.a = res(1 << 2, g_cpu.reg.a);
}

/* 0x98: Reset bit in register. */
void res_3_b(void)
{
    g_cpu.reg.b = res(1 << 3, g_cpu.reg.b);
}

/* 0x99: Reset bit in register. */
void res_3_c(void)
{
    g_cpu.reg.c = res(1 << 3, g_cpu.reg.c);
}

/* 0x9a: Reset bit in register. */
void res_3_d(void)
{
    g_cpu.reg.d = res(1 << 3, g_cpu.reg.d);
}

/* 0x9b: Reset bit in register. */
void res_3_e(void)
{
    g_cpu.reg.e = res(1 << 3, g_cpu.reg.e);
}

/* 0x9c: Reset bit in register. */
void res_3_h(void)
{
    g_cpu.reg.h = res(1 << 3, g_cpu.reg.h);
}

/* 0x9d: Reset bit in register. */
void res_3_l(void)
{
    g_cpu.reg.l = res(1 << 3, g_cpu.reg.l);
}

/* 0x9e: Reset bit in register. */
void res_3_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 3, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0x9f: Reset bit in register. */
void res_3_a(void)
{
    g_cpu.reg.a = res(1 << 3, g_cpu.reg.a);
}

/* 0xa0: Reset bit in register. */
void res_4_b(void)
{
    g_cpu.reg.b = res(1 << 4, g_cpu.reg.b);
}

/* 0xa1: Reset bit in register. */
void res_4_c(void)
{
    g_cpu.reg.c = res(1 << 4, g_cpu.reg.c);
}

/* 0xa2: Reset bit in register. */
void res_4_d(void)
{
    g_cpu.reg.d = res(1 << 4, g_cpu.reg.d);
}

/* 0xa3: Reset bit in register. */
void res_4_e(void)
{
    g_cpu.reg.e = res(1 << 4, g_cpu.reg.e);
}

/* 0xa4: Reset bit in register. */
void res_4_h(void)
{
    g_cpu.reg.h = res(1 << 4, g_cpu.reg.h);
}

/* 0xa5: Reset bit in register. */
void res_4_l(void)
{
    g_cpu.reg.l = res(1 << 4, g_cpu.reg.l);
}

/* 0xa6: Reset bit in register. */
void res_4_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 4, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xa7: Reset bit in register. */
void res_4_a(void)
{
    g_cpu.reg.a = res(1 << 4, g_cpu.reg.a);
}

/* 0xa8: Reset bit in register. */
void res_5_b(void)
{
    g_cpu.reg.b = res(1 << 5, g_cpu.reg.b);
}

/* 0xa9: Reset bit in register. */
void res_5_c(void)
{
    g_cpu.reg.c = res(1 << 5, g_cpu.reg.c);
}

/* 0xaa: Reset bit in register. */
void res_5_d(void)
{
    g_cpu.reg.d = res(1 << 5, g_cpu.reg.d);
}

/* 0xab: Reset bit in register. */
void res_5_e(void)
{
    g_cpu.reg.e = res(1 << 5, g_cpu.reg.e);
}

/* 0xac: Reset bit in register. */
void res_5_h(void)
{
    g_cpu.reg.h = res(1 << 5, g_cpu.reg.h);
}

/* 0xad: Reset bit in register. */
void res_5_l(void)
{
    g_cpu.reg.l = res(1 << 5, g_cpu.reg.l);
}

/* 0xae: Reset bit in register. */
void res_5_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 5, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xaf: Reset bit in register. */
void res_5_a(void)
{
    g_cpu.reg.a = res(1 << 5, g_cpu.reg.a);
}

/* 0xb0: Reset bit in register. */
void res_6_b(void)
{
    g_cpu.reg.b = res(1 << 6, g_cpu.reg.b);
}

/* 0xb1: Reset bit in register. */
void res_6_c(void)
{
    g_cpu.reg.c = res(1 << 6, g_cpu.reg.c);
}

/* 0xb2: Reset bit in register. */
void res_6_d(void)
{
    g_cpu.reg.d = res(1 << 6, g_cpu.reg.d);
}

/* 0xb3: Reset bit in register. */
void res_6_e(void)
{
    g_cpu.reg.e = res(1 << 6, g_cpu.reg.e);
}

/* 0xb4: Reset bit in register. */
void res_6_h(void)
{
    g_cpu.reg.h = res(1 << 6, g_cpu.reg.h);
}

/* 0xb5: Reset bit in register. */
void res_6_l(void)
{
    g_cpu.reg.l = res(1 << 6, g_cpu.reg.l);
}

/* 0xb6: Reset bit in register. */
void res_6_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 6, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xb7: Reset bit in register. */
void res_6_a(void)
{
    g_cpu.reg.a = res(1 << 6, g_cpu.reg.a);
}

/* 0xb8: Reset bit in register. */
void res_7_b(void)
{
    g_cpu.reg.b = res(1 << 7, g_cpu.reg.b);
}

/* 0xb9: Reset bit in register. */
void res_7_c(void)
{
    g_cpu.reg.c = res(1 << 7, g_cpu.reg.c);
}

/* 0xba: Reset bit in register. */
void res_7_d(void)
{
    g_cpu.reg.d = res(1 << 7, g_cpu.reg.d);
}

/* 0xbb: Reset bit in register. */
void res_7_e(void)
{
    g_cpu.reg.e = res(1 << 7, g_cpu.reg.e);
}

/* 0xbc: Reset bit in register. */
void res_7_h(void)
{
    g_cpu.reg.h = res(1 << 7, g_cpu.reg.h);
}

/* 0xbd: Reset bit in register. */
void res_7_l(void)
{
    g_cpu.reg.l = res(1 << 7, g_cpu.reg.l);
}

/* 0xbe: Reset bit in register. */
void res_7_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, res(1 << 7, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xbf: Reset bit in register. */
void res_7_a(void)
{
    g_cpu.reg.a = res(1 << 7, g_cpu.reg.a);
}

/* 0xc0: Reset bit in register. */
void set_0_b(void)
{
    g_cpu.reg.b = set(1 << 0, g_cpu.reg.b);
}

/* 0xc1: Reset bit in register. */
void set_0_c(void)
{
    g_cpu.reg.c = set(1 << 0, g_cpu.reg.c);
}

/* 0xc2: Reset bit in register. */
void set_0_d(void)
{
    g_cpu.reg.d = set(1 << 0, g_cpu.reg.d);
}

/* 0xc3: Reset bit in register. */
void set_0_e(void)
{
    g_cpu.reg.e = set(1 << 0, g_cpu.reg.e);
}

/* 0xc4: Reset bit in register. */
void set_0_h(void)
{
    g_cpu.reg.h = set(1 << 0, g_cpu.reg.h);
}

/* 0xc5: Reset bit in register. */
void set_0_l(void)
{
    g_cpu.reg.l = set(1 << 0, g_cpu.reg.l);
}

/* 0xc6: Reset bit in register. */
void set_0_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 0, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xc7: Reset bit in register. */
void set_0_a(void)
{
    g_cpu.reg.a = set(1 << 0, g_cpu.reg.a);
}

/* 0xc8: Reset bit in register. */
void set_1_b(void)
{
    g_cpu.reg.b = set(1 << 1, g_cpu.reg.b);
}

/* 0xc9: Reset bit in register. */
void set_1_c(void)
{
    g_cpu.reg.c = set(1 << 1, g_cpu.reg.c);
}

/* 0xca: Reset bit in register. */
void set_1_d(void)
{
    g_cpu.reg.d = set(1 << 1, g_cpu.reg.d);
}

/* 0xcb: Reset bit in register. */
void set_1_e(void)
{
    g_cpu.reg.e = set(1 << 1, g_cpu.reg.e);
}

/* 0xcc: Reset bit in register. */
void set_1_h(void)
{
    g_cpu.reg.h = set(1 << 1, g_cpu.reg.h);
}

/* 0xcd: Reset bit in register. */
void set_1_l(void)
{
    g_cpu.reg.l = set(1 << 1, g_cpu.reg.l);
}

/* 0xce: Reset bit in register. */
void set_1_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 1, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xcf: Reset bit in register. */
void set_1_a(void)
{
    g_cpu.reg.a = set(1 << 1, g_cpu.reg.a);
}

/* 0xd0: Reset bit in register. */
void set_2_b(void)
{
    g_cpu.reg.b = set(1 << 2, g_cpu.reg.b);
}

/* 0xd1: Reset bit in register. */
void set_2_c(void)
{
    g_cpu.reg.c = set(1 << 2, g_cpu.reg.c);
}

/* 0xd2: Reset bit in register. */
void set_2_d(void)
{
    g_cpu.reg.d = set(1 << 2, g_cpu.reg.d);
}

/* 0xd3: Reset bit in register. */
void set_2_e(void)
{
    g_cpu.reg.e = set(1 << 2, g_cpu.reg.e);
}

/* 0xd4: Reset bit in register. */
void set_2_h(void)
{
    g_cpu.reg.h = set(1 << 2, g_cpu.reg.h);
}

/* 0xd5: Reset bit in register. */
void set_2_l(void)
{
    g_cpu.reg.l = set(1 << 2, g_cpu.reg.l);
}

/* 0xd6: Reset bit in register. */
void set_2_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 2, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xd7: Reset bit in register. */
void set_2_a(void)
{
    g_cpu.reg.a = set(1 << 2, g_cpu.reg.a);
}

/* 0xd8: Reset bit in register. */
void set_3_b(void)
{
    g_cpu.reg.b = set(1 << 3, g_cpu.reg.b);
}

/* 0xd9: Reset bit in register. */
void set_3_c(void)
{
    g_cpu.reg.c = set(1 << 3, g_cpu.reg.c);
}

/* 0xda: Reset bit in register. */
void set_3_d(void)
{
    g_cpu.reg.d = set(1 << 3, g_cpu.reg.d);
}

/* 0xdb: Reset bit in register. */
void set_3_e(void)
{
    g_cpu.reg.e = set(1 << 3, g_cpu.reg.e);
}

/* 0xdc: Reset bit in register. */
void set_3_h(void)
{
    g_cpu.reg.h = set(1 << 3, g_cpu.reg.h);
}

/* 0xdd: Reset bit in register. */
void set_3_l(void)
{
    g_cpu.reg.l = set(1 << 3, g_cpu.reg.l);
}

/* 0xde: Reset bit in register. */
void set_3_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 3, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xdf: Reset bit in register. */
void set_3_a(void)
{
    g_cpu.reg.a = set(1 << 3, g_cpu.reg.a);
}

/* 0xe0: Reset bit in register. */
void set_4_b(void)
{
    g_cpu.reg.b = set(1 << 4, g_cpu.reg.b);
}

/* 0xe1: Reset bit in register. */
void set_4_c(void)
{
    g_cpu.reg.c = set(1 << 4, g_cpu.reg.c);
}

/* 0xe2: Reset bit in register. */
void set_4_d(void)
{
    g_cpu.reg.d = set(1 << 4, g_cpu.reg.d);
}

/* 0xe3: Reset bit in register. */
void set_4_e(void)
{
    g_cpu.reg.e = set(1 << 4, g_cpu.reg.e);
}

/* 0xe4: Reset bit in register. */
void set_4_h(void)
{
    g_cpu.reg.h = set(1 << 4, g_cpu.reg.h);
}

/* 0xe5: Reset bit in register. */
void set_4_l(void)
{
    g_cpu.reg.l = set(1 << 4, g_cpu.reg.l);
}

/* 0xe6: Reset bit in register. */
void set_4_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 4, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xe7: Reset bit in register. */
void set_4_a(void)
{
    g_cpu.reg.a = set(1 << 4, g_cpu.reg.a);
}

/* 0xe8: Reset bit in register. */
void set_5_b(void)
{
    g_cpu.reg.b = set(1 << 5, g_cpu.reg.b);
}

/* 0xe9: Reset bit in register. */
void set_5_c(void)
{
    g_cpu.reg.c = set(1 << 5, g_cpu.reg.c);
}

/* 0xea: Reset bit in register. */
void set_5_d(void)
{
    g_cpu.reg.d = set(1 << 5, g_cpu.reg.d);
}

/* 0xeb: Reset bit in register. */
void set_5_e(void)
{
    g_cpu.reg.e = set(1 << 5, g_cpu.reg.e);
}

/* 0xec: Reset bit in register. */
void set_5_h(void)
{
    g_cpu.reg.h = set(1 << 5, g_cpu.reg.h);
}

/* 0xed: Reset bit in register. */
void set_5_l(void)
{
    g_cpu.reg.l = set(1 << 5, g_cpu.reg.l);
}

/* 0xee: Reset bit in register. */
void set_5_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 5, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xef: Reset bit in register. */
void set_5_a(void)
{
    g_cpu.reg.a = set(1 << 5, g_cpu.reg.a);
}

/* 0xf0: Reset bit in register. */
void set_6_b(void)
{
    g_cpu.reg.b = set(1 << 6, g_cpu.reg.b);
}

/* 0xf1: Reset bit in register. */
void set_6_c(void)
{
    g_cpu.reg.c = set(1 << 6, g_cpu.reg.c);
}

/* 0xf2: Reset bit in register. */
void set_6_d(void)
{
    g_cpu.reg.d = set(1 << 6, g_cpu.reg.d);
}

/* 0xf3: Reset bit in register. */
void set_6_e(void)
{
    g_cpu.reg.e = set(1 << 6, g_cpu.reg.e);
}

/* 0xf4: Reset bit in register. */
void set_6_h(void)
{
    g_cpu.reg.h = set(1 << 6, g_cpu.reg.h);
}

/* 0xf5: Reset bit in register. */
void set_6_l(void)
{
    g_cpu.reg.l = set(1 << 6, g_cpu.reg.l);
}

/* 0xf6: Reset bit in register. */
void set_6_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 6, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xf7: Reset bit in register. */
void set_6_a(void)
{
    g_cpu.reg.a = set(1 << 6, g_cpu.reg.a);
}

/* 0xf8: Reset bit in register. */
void set_7_b(void)
{
    g_cpu.reg.b = set(1 << 7, g_cpu.reg.b);
}

/* 0xf9: Reset bit in register. */
void set_7_c(void)
{
    g_cpu.reg.c = set(1 << 7, g_cpu.reg.c);
}

/* 0xfa: Reset bit in register. */
void set_7_d(void)
{
    g_cpu.reg.d = set(1 << 7, g_cpu.reg.d);
}

/* 0xfb: Reset bit in register. */
void set_7_e(void)
{
    g_cpu.reg.e = set(1 << 7, g_cpu.reg.e);
}

/* 0xfc: Reset bit in register. */
void set_7_h(void)
{
    g_cpu.reg.h = set(1 << 7, g_cpu.reg.h);
}

/* 0xfd: Reset bit in register. */
void set_7_l(void)
{
    g_cpu.reg.l = set(1 << 7, g_cpu.reg.l);
}

/* 0xfe: Reset bit in register. */
void set_7_hlp(void)
{
    mmu_write_byte(g_cpu.reg.hl, set(1 << 7, mmu_read_byte(g_cpu.reg.hl)));
}

/* 0xff: Reset bit in register. */
void set_7_a(void)
{
    g_cpu.reg.a = set(1 << 7, g_cpu.reg.a);
}
