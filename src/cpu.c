#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_ext_ops.h"
#include "cpu_opcodes.h"
#include "gpu.h"
#include "interrupt.h"
#include "mmu.h"
#include "timer.h"
#include "utils.h"

cpu_t g_cpu;

typedef void (*func0)(void);
typedef void (*func8)(uint8_t);
typedef void (*func16)(uint16_t);

typedef struct {
    const char *asm1;
    const char *asm2;
    uint8_t operand_length;
    func0 exec0;
    func8 exec1;
    func16 exec2;
} instruction_t;

const instruction_t g_instr[256] = {
    {"NOP", NULL, 0, nop, NULL, NULL},                    // 0x00
    {"LD BC, $", NULL, 2, NULL, NULL, ld_bc_nn},          // 0x01
    {"LD (BC), A", NULL, 0, ld_bcp_a, NULL, NULL},        // 0x02
    {"INC BC", NULL, 0, inc_bc, NULL, NULL},              // 0x03
    {"INC B", NULL, 0, inc_b, NULL, NULL},                // 0x04
    {"DEC B", NULL, 0, dec_b, NULL, NULL},                // 0x05
    {"LD B, $", NULL, 1, NULL, ld_b_n, NULL},             // 0x06
    {"RLCA", NULL, 0, rlca, NULL, NULL},                  // 0x07
    {"LD ($", "), SP", 2, NULL, NULL, ld_nnp_sp},         // 0x08
    {"ADD HL, BC", NULL, 0, add_hl_bc, NULL, NULL},       // 0x09
    {"LD A, (BC)", NULL, 0, ld_a_bcp, NULL, NULL},        // 0x0a
    {"DEC BC", NULL, 0, dec_bc, NULL, NULL},              // 0x0b
    {"INC C", NULL, 0, inc_c, NULL, NULL},                // 0x0c
    {"DEC C", NULL, 0, dec_c, NULL, NULL},                // 0x0d
    {"LD C, $", NULL, 1, NULL, ld_c_n, NULL},             // 0x0e
    {"RRCA", NULL, 0, rrca, NULL, NULL},                  // 0x0f
    {"STOP", NULL, 0, stop, NULL, NULL},                  // 0x10
    {"LD DE, $", NULL, 2, NULL, NULL, ld_de_nn},          // 0x11
    {"LD (DE), A", NULL, 0, ld_dep_a, NULL, NULL},        // 0x12
    {"INC DE", NULL, 0, inc_de, NULL, NULL},              // 0x13
    {"INC D", NULL, 0, inc_d, NULL, NULL},                // 0x14
    {"DEC D", NULL, 0, dec_d, NULL, NULL},                // 0x15
    {"LD D, $", NULL, 1, NULL, ld_d_n, NULL},             // 0x16
    {"RLA", NULL, 0, rla, NULL, NULL},                    // 0x17
    {"JR $", NULL, 1, NULL, jr_n, NULL},                  // 0x18
    {"ADD HL, DE", NULL, 0, add_hl_de, NULL, NULL},       // 0x19
    {"LD A, (DE)", NULL, 0, ld_a_dep, NULL, NULL},        // 0x1a
    {"DEC DE", NULL, 0, dec_de, NULL, NULL},              // 0x1b
    {"INC E", NULL, 0, inc_e, NULL, NULL},                // 0x1c
    {"DEC E", NULL, 0, dec_e, NULL, NULL},                // 0x1d
    {"LD E, $", NULL, 1, NULL, ld_e_n, NULL},             // 0x1e
    {"RRA", NULL, 0, rra, NULL, NULL},                    // 0x1f
    {"JR NZ, $", NULL, 1, NULL, jr_nz_n, NULL},           // 0x20
    {"LD HL, $", NULL, 2, NULL, NULL, ld_hl_nn},          // 0x21
    {"LD (HL+), A", NULL, 0, ldi_hlp_a, NULL, NULL},      // 0x22
    {"INC HL", NULL, 0, inc_hl, NULL, NULL},              // 0x23
    {"INC H", NULL, 0, inc_h, NULL, NULL},                // 0x24
    {"DEC H", NULL, 0, dec_h, NULL, NULL},                // 0x25
    {"LD H, $", NULL, 1, NULL, ld_h_n, NULL},             // 0x26
    {"DAA", NULL, 0, daa, NULL, NULL},                    // 0x27
    {"JR Z, $", NULL, 1, NULL, jr_z_n, NULL},             // 0x28
    {"ADD HL, HL", NULL, 0, add_hl_hl, NULL, NULL},       // 0x29
    {"LD A, (HL+)", NULL, 0, ldi_a_hlp, NULL, NULL},      // 0x2a
    {"DEC HL", NULL, 0, dec_hl, NULL, NULL},              // 0x2b
    {"INC L", NULL, 0, inc_l, NULL, NULL},                // 0x2c
    {"DEC L", NULL, 0, dec_l, NULL, NULL},                // 0x2d
    {"LD L, $", NULL, 1, NULL, ld_l_n, NULL},             // 0x2e
    {"CPL", NULL, 0, cpl, NULL, NULL},                    // 0x2f
    {"JR NC, $", NULL, 1, NULL, jr_nc_n, NULL},           // 0x30
    {"LD SP, $", NULL, 2, NULL, NULL, ld_sp_nn},          // 0x31
    {"LD (HL-), A", NULL, 0, ldd_hlp_a, NULL, NULL},      // 0x32
    {"INC SP", NULL, 0, inc_sp, NULL, NULL},              // 0x33
    {"INC (HL)", NULL, 0, inc_hlp, NULL, NULL},           // 0x34
    {"DEC (HL)", NULL, 0, dec_hlp, NULL, NULL},           // 0x35
    {"LD (HL), $", NULL, 1, NULL, ld_hlp_n, NULL},        // 0x36
    {"SCF", NULL, 0, scf, NULL, NULL},                    // 0x37
    {"JR C, $", NULL, 1, NULL, jr_c_n, NULL},             // 0x38
    {"ADD HL, SP", NULL, 0, add_hl_sp, NULL, NULL},       // 0x39
    {"LDD A, (HL)", NULL, 0, ldd_a_hlp, NULL, NULL},      // 0x3a
    {"DEC SP", NULL, 0, dec_sp, NULL, NULL},              // 0x3b
    {"INC A", NULL, 0, inc_a, NULL, NULL},                // 0x3c
    {"DEC A", NULL, 0, dec_a, NULL, NULL},                // 0x3d
    {"LD A, $", NULL, 1, NULL, ld_a_n, NULL},             // 0x3e
    {"CCF", NULL, 0, ccf, NULL, NULL},                    // 0x3f
    {"LD B, B", NULL, 0, nop, NULL, NULL},                // 0x40
    {"LD B, C", NULL, 0, ld_b_c, NULL, NULL},             // 0x41
    {"LD B, D", NULL, 0, ld_b_d, NULL, NULL},             // 0x42
    {"LD B, E", NULL, 0, ld_b_e, NULL, NULL},             // 0x43
    {"LD B, H", NULL, 0, ld_b_h, NULL, NULL},             // 0x44
    {"LD B, L", NULL, 0, ld_b_l, NULL, NULL},             // 0x45
    {"LD B, (HL)", NULL, 0, ld_b_hlp, NULL, NULL},        // 0x46
    {"LD B, A", NULL, 0, ld_b_a, NULL, NULL},             // 0x47
    {"LD C, B", NULL, 0, ld_c_b, NULL, NULL},             // 0x48
    {"LD C, C", NULL, 0, nop, NULL, NULL},                // 0x49
    {"LD C, D", NULL, 0, ld_c_d, NULL, NULL},             // 0x4a
    {"LD C, E", NULL, 0, ld_c_e, NULL, NULL},             // 0x4b
    {"LD C, H", NULL, 0, ld_c_h, NULL, NULL},             // 0x4c
    {"LD C, L", NULL, 0, ld_c_l, NULL, NULL},             // 0x4d
    {"LD C, (HL)", NULL, 0, ld_c_hlp, NULL, NULL},        // 0x4e
    {"LD C, A", NULL, 0, ld_c_a, NULL, NULL},             // 0x4f
    {"LD D, B", NULL, 0, ld_d_b, NULL, NULL},             // 0x50
    {"LD D, C", NULL, 0, ld_d_c, NULL, NULL},             // 0x51
    {"LD D, D", NULL, 0, nop, NULL, NULL},                // 0x52
    {"LD D, E", NULL, 0, ld_d_e, NULL, NULL},             // 0x53
    {"LD D, H", NULL, 0, ld_d_h, NULL, NULL},             // 0x54
    {"LD D, L", NULL, 0, ld_d_l, NULL, NULL},             // 0x55
    {"LD D, (HL)", NULL, 0, ld_d_hlp, NULL, NULL},        // 0x56
    {"LD D, A", NULL, 0, ld_d_a, NULL, NULL},             // 0x57
    {"LD E, B", NULL, 0, ld_e_b, NULL, NULL},             // 0x58
    {"LD E, C", NULL, 0, ld_e_c, NULL, NULL},             // 0x59
    {"LD E, D", NULL, 0, ld_e_d, NULL, NULL},             // 0x5a
    {"LD E, E", NULL, 0, nop, NULL, NULL},                // 0x5b
    {"LD E, H", NULL, 0, ld_e_h, NULL, NULL},             // 0x5c
    {"LD E, L", NULL, 0, ld_e_l, NULL, NULL},             // 0x5d
    {"LD E, (HL)", NULL, 0, ld_e_hlp, NULL, NULL},        // 0x5e
    {"LD E, A", NULL, 0, ld_e_a, NULL, NULL},             // 0x5f
    {"LD H, B", NULL, 0, ld_h_b, NULL, NULL},             // 0x60
    {"LD H, C", NULL, 0, ld_h_c, NULL, NULL},             // 0x61
    {"LD H, D", NULL, 0, ld_h_d, NULL, NULL},             // 0x62
    {"LD H, E", NULL, 0, ld_h_e, NULL, NULL},             // 0x63
    {"LD H, H", NULL, 0, nop, NULL, NULL},                // 0x64
    {"LD H, L", NULL, 0, ld_h_l, NULL, NULL},             // 0x65
    {"LD H, (HL)", NULL, 0, ld_h_hlp, NULL, NULL},        // 0x66
    {"LD H, A", NULL, 0, ld_h_a, NULL, NULL},             // 0x67
    {"LD L, B", NULL, 0, ld_l_b, NULL, NULL},             // 0x68
    {"LD L, C", NULL, 0, ld_l_c, NULL, NULL},             // 0x69
    {"LD L, D", NULL, 0, ld_l_d, NULL, NULL},             // 0x6a
    {"LD L, E", NULL, 0, ld_l_e, NULL, NULL},             // 0x6b
    {"LD L, H", NULL, 0, ld_l_h, NULL, NULL},             // 0x6c
    {"LD L, L", NULL, 0, nop, NULL, NULL},                // 0x6d
    {"LD L, (HL)", NULL, 0, ld_l_hlp, NULL, NULL},        // 0x6e
    {"LD L, A", NULL, 0, ld_l_a, NULL, NULL},             // 0x6f
    {"LD (HL), B", NULL, 0, ld_hlp_b, NULL, NULL},        // 0x70
    {"LD (HL), C", NULL, 0, ld_hlp_c, NULL, NULL},        // 0x71
    {"LD (HL), D", NULL, 0, ld_hlp_d, NULL, NULL},        // 0x72
    {"LD (HL), E", NULL, 0, ld_hlp_e, NULL, NULL},        // 0x73
    {"LD (HL), H", NULL, 0, ld_hlp_h, NULL, NULL},        // 0x74
    {"LD (HL), L", NULL, 0, ld_hlp_l, NULL, NULL},        // 0x75
    {"HALT", NULL, 0, halt, NULL, NULL},                  // 0x76
    {"LD (HL), A", NULL, 0, ld_hlp_a, NULL, NULL},        // 0x77
    {"LD A, B", NULL, 0, ld_a_b, NULL, NULL},             // 0x78
    {"LD A, C", NULL, 0, ld_a_c, NULL, NULL},             // 0x79
    {"LD A, D", NULL, 0, ld_a_d, NULL, NULL},             // 0x7a
    {"LD A, E", NULL, 0, ld_a_e, NULL, NULL},             // 0x7b
    {"LD A, H", NULL, 0, ld_a_h, NULL, NULL},             // 0x7c
    {"LD A, L", NULL, 0, ld_a_l, NULL, NULL},             // 0x7d
    {"LD A, (HL)", NULL, 0, ld_a_hlp, NULL, NULL},        // 0x7e
    {"LD A, A", NULL, 0, nop, NULL, NULL},                // 0x7f
    {"ADD A, B", NULL, 0, add_a_b, NULL, NULL},           // 0x80
    {"ADD A, C", NULL, 0, add_a_c, NULL, NULL},           // 0x81
    {"ADD A, D", NULL, 0, add_a_d, NULL, NULL},           // 0x82
    {"ADD A, E", NULL, 0, add_a_e, NULL, NULL},           // 0x83
    {"ADD A, H", NULL, 0, add_a_h, NULL, NULL},           // 0x84
    {"ADD A, L", NULL, 0, add_a_l, NULL, NULL},           // 0x85
    {"ADD A, (HL)", NULL, 0, add_a_hlp, NULL, NULL},      // 0x86
    {"ADD A", NULL, 0, add_a_a, NULL, NULL},              // 0x87
    {"ADC B", NULL, 0, adc_b, NULL, NULL},                // 0x88
    {"ADC C", NULL, 0, adc_c, NULL, NULL},                // 0x89
    {"ADC D", NULL, 0, adc_d, NULL, NULL},                // 0x8a
    {"ADC E", NULL, 0, adc_e, NULL, NULL},                // 0x8b
    {"ADC H", NULL, 0, adc_h, NULL, NULL},                // 0x8c
    {"ADC L", NULL, 0, adc_l, NULL, NULL},                // 0x8d
    {"ADC (HL)", NULL, 0, adc_hlp, NULL, NULL},           // 0x8e
    {"ADC A", NULL, 0, adc_a, NULL, NULL},                // 0x8f
    {"SUB B", NULL, 0, sub_b, NULL, NULL},                // 0x90
    {"SUB C", NULL, 0, sub_c, NULL, NULL},                // 0x91
    {"SUB D", NULL, 0, sub_d, NULL, NULL},                // 0x92
    {"SUB E", NULL, 0, sub_e, NULL, NULL},                // 0x93
    {"SUB H", NULL, 0, sub_h, NULL, NULL},                // 0x94
    {"SUB L", NULL, 0, sub_l, NULL, NULL},                // 0x95
    {"SUB (HL)", NULL, 0, sub_hlp, NULL, NULL},           // 0x96
    {"SUB A", NULL, 0, sub_a, NULL, NULL},                // 0x97
    {"SBC B", NULL, 0, sbc_b, NULL, NULL},                // 0x98
    {"SBC C", NULL, 0, sbc_c, NULL, NULL},                // 0x99
    {"SBC D", NULL, 0, sbc_d, NULL, NULL},                // 0x9a
    {"SBC E", NULL, 0, sbc_e, NULL, NULL},                // 0x9b
    {"SBC H", NULL, 0, sbc_h, NULL, NULL},                // 0x9c
    {"SBC L", NULL, 0, sbc_l, NULL, NULL},                // 0x9d
    {"SBC (HL)", NULL, 0, sbc_hlp, NULL, NULL},           // 0x9e
    {"SBC A", NULL, 0, sbc_a, NULL, NULL},                // 0x9f
    {"AND B", NULL, 0, and_b, NULL, NULL},                // 0xa0
    {"AND C", NULL, 0, and_c, NULL, NULL},                // 0xa1
    {"AND D", NULL, 0, and_d, NULL, NULL},                // 0xa2
    {"AND E", NULL, 0, and_e, NULL, NULL},                // 0xa3
    {"AND H", NULL, 0, and_h, NULL, NULL},                // 0xa4
    {"AND L", NULL, 0, and_l, NULL, NULL},                // 0xa5
    {"AND (HL)", NULL, 0, and_hlp, NULL, NULL},           // 0xa6
    {"AND A", NULL, 0, and_a, NULL, NULL},                // 0xa7
    {"XOR B", NULL, 0, xor_b, NULL, NULL},                // 0xa8
    {"XOR C", NULL, 0, xor_c, NULL, NULL},                // 0xa9
    {"XOR D", NULL, 0, xor_d, NULL, NULL},                // 0xaa
    {"XOR E", NULL, 0, xor_e, NULL, NULL},                // 0xab
    {"XOR H", NULL, 0, xor_h, NULL, NULL},                // 0xac
    {"XOR L", NULL, 0, xor_l, NULL, NULL},                // 0xad
    {"XOR (HL)", NULL, 0, xor_hlp, NULL, NULL},           // 0xae
    {"XOR A", NULL, 0, xor_a, NULL, NULL},                // 0xaf
    {"OR B", NULL, 0, or_b, NULL, NULL},                  // 0xb0
    {"OR C", NULL, 0, or_c, NULL, NULL},                  // 0xb1
    {"OR D", NULL, 0, or_d, NULL, NULL},                  // 0xb2
    {"OR E", NULL, 0, or_e, NULL, NULL},                  // 0xb3
    {"OR H", NULL, 0, or_h, NULL, NULL},                  // 0xb4
    {"OR L", NULL, 0, or_l, NULL, NULL},                  // 0xb5
    {"OR (HL)", NULL, 0, or_hlp, NULL, NULL},             // 0xb6
    {"OR A", NULL, 0, or_a, NULL, NULL},                  // 0xb7
    {"CP B", NULL, 0, cp_b, NULL, NULL},                  // 0xb8
    {"CP C", NULL, 0, cp_c, NULL, NULL},                  // 0xb9
    {"CP D", NULL, 0, cp_d, NULL, NULL},                  // 0xba
    {"CP E", NULL, 0, cp_e, NULL, NULL},                  // 0xbb
    {"CP H", NULL, 0, cp_h, NULL, NULL},                  // 0xbc
    {"CP L", NULL, 0, cp_l, NULL, NULL},                  // 0xbd
    {"CP (HL)", NULL, 0, cp_hlp, NULL, NULL},             // 0xbe
    {"CP A", NULL, 0, cp_a, NULL, NULL},                  // 0xbf
    {"RET NZ", NULL, 0, ret_nz, NULL, NULL},              // 0xc0
    {"POP BC", NULL, 0, pop_bc, NULL, NULL},              // 0xc1
    {"JP NZ, $", NULL, 2, NULL, NULL, jp_nz_nn},          // 0xc2
    {"JP $", NULL, 2, NULL, NULL, jp_nn},                 // 0xc3
    {"CALL NZ, $", NULL, 2, NULL, NULL, call_nz_nn},      // 0xc4
    {"PUSH BC", NULL, 0, push_bc, NULL, NULL},            // 0xc5
    {"ADD A, $", NULL, 1, NULL, add_a_n, NULL},           // 0xc6
    {"RST $00", NULL, 0, rst_00, NULL, NULL},             // 0xc7
    {"RET Z", NULL, 0, ret_z, NULL, NULL},                // 0xc8
    {"RET", NULL, 0, ret, NULL, NULL},                    // 0xc9
    {"JP Z, $", NULL, 2, NULL, NULL, jp_z_nn},            // 0xca
    {"CB ", NULL, 1, NULL, cb_n, NULL},                   // 0xcb
    {"CALL Z, $", NULL, 2, NULL, NULL, call_z_nn},        // 0xcc
    {"CALL $", NULL, 2, NULL, NULL, call_nn},             // 0xcd
    {"ADC $", NULL, 1, NULL, adc_n, NULL},                // 0xce
    {"RST $08", NULL, 0, rst_08, NULL, NULL},             // 0xcf
    {"RET NC", NULL, 0, ret_nc, NULL, NULL},              // 0xd0
    {"POP DE", NULL, 0, pop_de, NULL, NULL},              // 0xd1
    {"JP NC, $", NULL, 2, NULL, NULL, jp_nc_nn},          // 0xd2
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xd3
    {"CALL NC, $", NULL, 2, NULL, NULL, call_nc_nn},      // 0xd4
    {"PUSH DE", NULL, 0, push_de, NULL, NULL},            // 0xd5
    {"SUB $", NULL, 1, NULL, sub_n, NULL},                // 0xd6
    {"RST $10", NULL, 0, rst_10, NULL, NULL},             // 0xd7
    {"RET C", NULL, 0, ret_c, NULL, NULL},                // 0xd8
    {"RETI", NULL, 0, reti, NULL, NULL},                  // 0xd9
    {"JP C, $", NULL, 2, NULL, NULL, jp_c_nn},            // 0xda
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xdb
    {"CALL C, $", NULL, 2, NULL, NULL, call_c_nn},        // 0xdc
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xdd
    {"SBC $", NULL, 1, NULL, sbc_n, NULL},                // 0xde
    {"RST $18", NULL, 0, rst_18, NULL, NULL},             // 0xdf
    {"LDH ($FF00 + $", "), A", 1, NULL, ldh_n_a, NULL},   // 0xe0
    {"POP HL", NULL, 0, pop_hl, NULL, NULL},              // 0xe1
    {"LD ($FF00 + C), A", NULL, 0, ld_cp_a, NULL, NULL},  // 0xe2
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xe3
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xe4
    {"PUSH HL", NULL, 0, push_hl, NULL, NULL},            // 0xe5
    {"AND $", NULL, 1, NULL, and_n, NULL},                // 0xe6
    {"RST $20", NULL, 0, rst_20, NULL, NULL},             // 0xe7
    {"ADD SP,$", NULL, 1, NULL, add_sp_n, NULL},          // 0xe8
    {"JP HL", NULL, 0, jp_hl, NULL, NULL},                // 0xe9
    {"LD ($", "), A", 2, NULL, NULL, ld_nnp_a},           // 0xea
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xeb
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xec
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xed
    {"XOR $", NULL, 1, NULL, xor_n, NULL},                // 0xee
    {"RST $28", NULL, 0, rst_28, NULL, NULL},             // 0xef
    {"LDH A, ($FF00 + $", ")", 1, NULL, ldh_a_n, NULL},   // 0xf0
    {"POP AF", NULL, 0, pop_af, NULL, NULL},              // 0xf1
    {"LD A, ($FF00 + C)", NULL, 0, ld_a_cp, NULL, NULL},  // 0xf2
    {"DI", NULL, 0, di, NULL, NULL},                      // 0xf3
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xf4
    {"PUSH AF", NULL, 0, push_af, NULL, NULL},            // 0xf5
    {"OR $", NULL, 1, NULL, or_n, NULL},                  // 0xf6
    {"RST $30", NULL, 0, rst_30, NULL, NULL},             // 0xf7
    {"LDHL, SP+$", NULL, 1, NULL, ldhl_sp_n, NULL},       // 0xf8
    {"LD SP, HL", NULL, 0, ld_sp_hl, NULL, NULL},         // 0xf9
    {"LD A, ($", ")", 2, NULL, NULL, ld_a_nnp},           // 0xfa
    {"EI", NULL, 0, ei, NULL, NULL},                      // 0xfb
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xfc
    {"UNKNOWN", NULL, 0, undefined, NULL, NULL},          // 0xfd
    {"CP $", NULL, 1, NULL, cp_n, NULL},                  // 0xfe
    {"RST $38", NULL, 0, rst_38, NULL, NULL},             // 0xff
};

/**
 * Gameboy CPU (LR35902) instruction cycles divided by 4.
 * Values based in the table found in:
 * http://pastraiser.com/cpu/gameboy/gameboy_opcodes.html
 */
const uint8_t instruction_ticks[256] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,  // 0x0_
    1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,  // 0x1_
    0, 3, 2, 2, 1, 1, 2, 1, 0, 2, 2, 2, 1, 1, 2, 1,  // 0x2_
    0, 3, 2, 2, 3, 3, 3, 1, 0, 2, 2, 2, 1, 1, 2, 1,  // 0x3_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x4_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x5_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x6_
    2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x7_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x8_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0x9_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0xa_
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,  // 0xb_
    0, 3, 0, 4, 0, 4, 2, 4, 0, 4, 0, 0, 0, 3, 2, 4,  // 0xc_
    0, 3, 0, 0, 0, 4, 2, 4, 0, 4, 0, 0, 0, 0, 2, 4,  // 0xd_
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,  // 0xe_
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4,  // 0xf_
};

cpu_t *cpu_get_instance(void)
{
    return &g_cpu;
}

void cpu_debug_flags(char *str, size_t size)
{
    const char *fc = FLAG_IS_SET(FLAG_C) ? " [CARRY]" : "";
    const char *fh = FLAG_IS_SET(FLAG_H) ? " [HALF CARRY]" : "";
    const char *fn = FLAG_IS_SET(FLAG_N) ? " [SUBTRACT]" : "";
    const char *fz = FLAG_IS_SET(FLAG_Z) ? " [ZERO]" : "";
    snprintf(str, size, "FLAGS:%s%s%s%s", fc, fh, fn, fz);
}

static void cpu_debug_instr(uint16_t pc, char *str, size_t size)
{
    /* Get next opcode. */
    uint8_t opcode = mmu_read_byte(pc++);
    uint8_t oper_length = g_instr[opcode].operand_length;
    const char *asm1 = g_instr[opcode].asm1;
    const char *asm2 = g_instr[opcode].asm2 ? g_instr[opcode].asm2 : "";

    if (oper_length == 0) {
        snprintf(str, size, "0x%02x: %s", opcode, asm1);
    } else if (oper_length == 1) {
        uint8_t operand = mmu_read_byte(pc);
        snprintf(str, size, "0x%02x: %s%02x%s", opcode, asm1, operand, asm2);
    } else {
        uint16_t operand = mmu_read_word(pc);
        snprintf(str, size, "0x%02x: %s%04x%s", opcode, asm1, operand, asm2);
    }
}

#ifdef DEBUG
char debug_str[100];

static char *cpu_debug_instr0(char *str, uint8_t opcode)
{
    sprintf(str, "0x%02x:   %s", opcode, g_instr[opcode].asm1);
    return str;
}

static char *cpu_debug_instr1(char *str, uint8_t opcode, uint8_t operand)
{
    const char *asm1 = g_instr[opcode].asm1;
    const char *asm2 = g_instr[opcode].asm2 ? g_instr[opcode].asm2 : "";
    if (opcode == 0xcb) {
        print_ext_ops(str, operand);
    } else {
        sprintf(str, "0x%02x:   %s%02x%s", opcode, asm1, operand, asm2);
    }
    return str;
}

static char *cpu_debug_instr2(char *str, uint8_t opcode, uint16_t operand)
{
    const char *asm1 = g_instr[opcode].asm1;
    const char *asm2 = g_instr[opcode].asm2 ? g_instr[opcode].asm2 : "";
    sprintf(str, "0x%02x:   %s%04x%s", opcode, asm1, operand, asm2);
    return str;
}
#endif

void cpu_debug_last_instr(char *str, size_t size)
{
    snprintf(str, size, "Last: ");
    cpu_debug_instr(g_cpu.last_pc, str + strlen(str), size);
}

void cpu_debug_next_instr(char *str, size_t size)
{
    snprintf(str, size, "Next: ");
    cpu_debug_instr(g_cpu.reg.pc, str + strlen(str), size);
}

void cpu_debug_cycles(char *str, size_t size)
{
    snprintf(str, size, "Cycle: %u", g_cpu.cycle);
}

void cpu_dump(void)
{
    printf("Dumping CPU info:\n");
    printf("PC:0x%04x SP:0x%04x\n", g_cpu.reg.pc, g_cpu.reg.sp);
    printf("AF:0x%04x BC:0x%04x DE:0x%04x HL:0x%04x\n", g_cpu.reg.af,
           g_cpu.reg.bc, g_cpu.reg.de, g_cpu.reg.hl);
#if 0
    for (uint32_t reg = 0xff00; reg <= 0xffff; ++reg) {
        printf("[0x%04x]=0x%02x\n", reg, mmu_read_byte((uint16_t)reg));
    }
#endif
}

int cpu_init(const char *rom_path)
{
    memset(&g_cpu, 0x0, sizeof(g_cpu));
    timer_init();
    return mmu_init(rom_path);
}

static uint8_t cpu_fetch_opcode(void)
{
    uint8_t op = mmu_read_byte(g_cpu.reg.pc);
    g_cpu.last_pc = g_cpu.reg.pc++;
    return op;
}

static void cpu_decode_opcode(uint8_t opcode)
{
    uint8_t oper_length = g_instr[opcode].operand_length;
    printd("PC:0x%04x SP:0x%04x AF:0x%04x BC:0x%04x DE:0x%04x HL:0x%04x: ",
           g_cpu.last_pc, g_cpu.reg.sp, g_cpu.reg.af, g_cpu.reg.bc,
           g_cpu.reg.de, g_cpu.reg.hl);
    if (oper_length == 0) {
        printd("%s\n", cpu_debug_instr0(debug_str, opcode));
        g_instr[opcode].exec0();
    } else if (oper_length == 1) {
        uint8_t operand = mmu_read_byte(g_cpu.reg.pc);
        g_cpu.reg.pc = (uint16_t)(g_cpu.reg.pc + 1);
        printd("%s\n", cpu_debug_instr1(debug_str, opcode, operand));
        g_instr[opcode].exec1(operand);
    } else if (oper_length == 2) {
        uint16_t operand = mmu_read_word(g_cpu.reg.pc);
        g_cpu.reg.pc = (uint16_t)(g_cpu.reg.pc + 2);
        printd("%s\n", cpu_debug_instr2(debug_str, opcode, operand));
        g_instr[opcode].exec2(operand);
    } else {
        fprintf(stderr, "ERROR: invalid operand length!\n");
        exit(EXIT_FAILURE);
    }
    g_cpu.clock.step += instruction_ticks[opcode];
    g_cpu.cycle++;
}

void cpu_emulate_cycle(void)
{
    uint8_t opcode = cpu_fetch_opcode();
    cpu_decode_opcode(opcode);
    interrupt_step();
    gpu_step(g_cpu.clock.step);
    timer_step(g_cpu.clock.step);
    g_cpu.clock.main += g_cpu.clock.step;
}
