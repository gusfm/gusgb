#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cartridge/cart.h"
#include "clock.h"
#include "cpu_ext_ops.h"
#include "cpu_opcodes.h"
#include "debug.h"
#include "gpu.h"
#include "interrupt.h"
#include "mmu.h"

cpu_t CPU;

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
    {"nop", NULL, 0, nop, NULL, NULL},                   // 0x00
    {"ld bc, $", NULL, 2, NULL, NULL, ld_bc_nn},         // 0x01
    {"ld (bc), a", NULL, 0, ld_bcp_a, NULL, NULL},       // 0x02
    {"inc bc", NULL, 0, inc_bc, NULL, NULL},             // 0x03
    {"inc b", NULL, 0, inc_b, NULL, NULL},               // 0x04
    {"dec b", NULL, 0, dec_b, NULL, NULL},               // 0x05
    {"ld b, $", NULL, 1, NULL, ld_b_n, NULL},            // 0x06
    {"rlca", NULL, 0, rlca, NULL, NULL},                 // 0x07
    {"ld ($", "), sp", 2, NULL, NULL, ld_nnp_sp},        // 0x08
    {"add hl, bc", NULL, 0, add_hl_bc, NULL, NULL},      // 0x09
    {"ld a, (bc)", NULL, 0, ld_a_bcp, NULL, NULL},       // 0x0a
    {"dec bc", NULL, 0, dec_bc, NULL, NULL},             // 0x0b
    {"inc c", NULL, 0, inc_c, NULL, NULL},               // 0x0c
    {"dec c", NULL, 0, dec_c, NULL, NULL},               // 0x0d
    {"ld c, $", NULL, 1, NULL, ld_c_n, NULL},            // 0x0e
    {"rrca", NULL, 0, rrca, NULL, NULL},                 // 0x0f
    {"stop", NULL, 0, stop, NULL, NULL},                 // 0x10
    {"ld de, $", NULL, 2, NULL, NULL, ld_de_nn},         // 0x11
    {"ld (de), a", NULL, 0, ld_dep_a, NULL, NULL},       // 0x12
    {"inc de", NULL, 0, inc_de, NULL, NULL},             // 0x13
    {"inc d", NULL, 0, inc_d, NULL, NULL},               // 0x14
    {"dec d", NULL, 0, dec_d, NULL, NULL},               // 0x15
    {"ld d, $", NULL, 1, NULL, ld_d_n, NULL},            // 0x16
    {"rla", NULL, 0, rla, NULL, NULL},                   // 0x17
    {"jr $", NULL, 1, NULL, jr_n, NULL},                 // 0x18
    {"add hl, de", NULL, 0, add_hl_de, NULL, NULL},      // 0x19
    {"ld a, (de)", NULL, 0, ld_a_dep, NULL, NULL},       // 0x1a
    {"dec de", NULL, 0, dec_de, NULL, NULL},             // 0x1b
    {"inc e", NULL, 0, inc_e, NULL, NULL},               // 0x1c
    {"dec e", NULL, 0, dec_e, NULL, NULL},               // 0x1d
    {"ld e, $", NULL, 1, NULL, ld_e_n, NULL},            // 0x1e
    {"rra", NULL, 0, rra, NULL, NULL},                   // 0x1f
    {"jr nz, $", NULL, 1, NULL, jr_nz_n, NULL},          // 0x20
    {"ld hl, $", NULL, 2, NULL, NULL, ld_hl_nn},         // 0x21
    {"ldi (hl), a", NULL, 0, ldi_hlp_a, NULL, NULL},     // 0x22
    {"inc hl", NULL, 0, inc_hl, NULL, NULL},             // 0x23
    {"inc h", NULL, 0, inc_h, NULL, NULL},               // 0x24
    {"dec h", NULL, 0, dec_h, NULL, NULL},               // 0x25
    {"ld h, $", NULL, 1, NULL, ld_h_n, NULL},            // 0x26
    {"daa", NULL, 0, daa, NULL, NULL},                   // 0x27
    {"jr z, $", NULL, 1, NULL, jr_z_n, NULL},            // 0x28
    {"add hl, hl", NULL, 0, add_hl_hl, NULL, NULL},      // 0x29
    {"ldi a, (hl)", NULL, 0, ldi_a_hlp, NULL, NULL},     // 0x2a
    {"dec hl", NULL, 0, dec_hl, NULL, NULL},             // 0x2b
    {"inc l", NULL, 0, inc_l, NULL, NULL},               // 0x2c
    {"dec l", NULL, 0, dec_l, NULL, NULL},               // 0x2d
    {"ld l, $", NULL, 1, NULL, ld_l_n, NULL},            // 0x2e
    {"cpl", NULL, 0, cpl, NULL, NULL},                   // 0x2f
    {"jr nc, $", NULL, 1, NULL, jr_nc_n, NULL},          // 0x30
    {"ld sp, $", NULL, 2, NULL, NULL, ld_sp_nn},         // 0x31
    {"ldd (hl), a", NULL, 0, ldd_hlp_a, NULL, NULL},     // 0x32
    {"inc sp", NULL, 0, inc_sp, NULL, NULL},             // 0x33
    {"inc (hl)", NULL, 0, inc_hlp, NULL, NULL},          // 0x34
    {"dec (hl)", NULL, 0, dec_hlp, NULL, NULL},          // 0x35
    {"ld (hl), $", NULL, 1, NULL, ld_hlp_n, NULL},       // 0x36
    {"scf", NULL, 0, scf, NULL, NULL},                   // 0x37
    {"jr c, $", NULL, 1, NULL, jr_c_n, NULL},            // 0x38
    {"add hl, sp", NULL, 0, add_hl_sp, NULL, NULL},      // 0x39
    {"ldd a, (hl)", NULL, 0, ldd_a_hlp, NULL, NULL},     // 0x3a
    {"dec sp", NULL, 0, dec_sp, NULL, NULL},             // 0x3b
    {"inc a", NULL, 0, inc_a, NULL, NULL},               // 0x3c
    {"dec a", NULL, 0, dec_a, NULL, NULL},               // 0x3d
    {"ld a, $", NULL, 1, NULL, ld_a_n, NULL},            // 0x3e
    {"ccf", NULL, 0, ccf, NULL, NULL},                   // 0x3f
    {"ld b, b", NULL, 0, nop, NULL, NULL},               // 0x40
    {"ld b, c", NULL, 0, ld_b_c, NULL, NULL},            // 0x41
    {"ld b, d", NULL, 0, ld_b_d, NULL, NULL},            // 0x42
    {"ld b, e", NULL, 0, ld_b_e, NULL, NULL},            // 0x43
    {"ld b, h", NULL, 0, ld_b_h, NULL, NULL},            // 0x44
    {"ld b, l", NULL, 0, ld_b_l, NULL, NULL},            // 0x45
    {"ld b, (hl)", NULL, 0, ld_b_hlp, NULL, NULL},       // 0x46
    {"ld b, a", NULL, 0, ld_b_a, NULL, NULL},            // 0x47
    {"ld c, b", NULL, 0, ld_c_b, NULL, NULL},            // 0x48
    {"ld c, c", NULL, 0, nop, NULL, NULL},               // 0x49
    {"ld c, d", NULL, 0, ld_c_d, NULL, NULL},            // 0x4a
    {"ld c, e", NULL, 0, ld_c_e, NULL, NULL},            // 0x4b
    {"ld c, h", NULL, 0, ld_c_h, NULL, NULL},            // 0x4c
    {"ld c, l", NULL, 0, ld_c_l, NULL, NULL},            // 0x4d
    {"ld c, (hl)", NULL, 0, ld_c_hlp, NULL, NULL},       // 0x4e
    {"ld c, a", NULL, 0, ld_c_a, NULL, NULL},            // 0x4f
    {"ld d, b", NULL, 0, ld_d_b, NULL, NULL},            // 0x50
    {"ld d, c", NULL, 0, ld_d_c, NULL, NULL},            // 0x51
    {"ld d, d", NULL, 0, nop, NULL, NULL},               // 0x52
    {"ld d, e", NULL, 0, ld_d_e, NULL, NULL},            // 0x53
    {"ld d, h", NULL, 0, ld_d_h, NULL, NULL},            // 0x54
    {"ld d, l", NULL, 0, ld_d_l, NULL, NULL},            // 0x55
    {"ld d, (hl)", NULL, 0, ld_d_hlp, NULL, NULL},       // 0x56
    {"ld d, a", NULL, 0, ld_d_a, NULL, NULL},            // 0x57
    {"ld e, b", NULL, 0, ld_e_b, NULL, NULL},            // 0x58
    {"ld e, c", NULL, 0, ld_e_c, NULL, NULL},            // 0x59
    {"ld e, d", NULL, 0, ld_e_d, NULL, NULL},            // 0x5a
    {"ld e, e", NULL, 0, nop, NULL, NULL},               // 0x5b
    {"ld e, h", NULL, 0, ld_e_h, NULL, NULL},            // 0x5c
    {"ld e, l", NULL, 0, ld_e_l, NULL, NULL},            // 0x5d
    {"ld e, (hl)", NULL, 0, ld_e_hlp, NULL, NULL},       // 0x5e
    {"ld e, a", NULL, 0, ld_e_a, NULL, NULL},            // 0x5f
    {"ld h, b", NULL, 0, ld_h_b, NULL, NULL},            // 0x60
    {"ld h, c", NULL, 0, ld_h_c, NULL, NULL},            // 0x61
    {"ld h, d", NULL, 0, ld_h_d, NULL, NULL},            // 0x62
    {"ld h, e", NULL, 0, ld_h_e, NULL, NULL},            // 0x63
    {"ld h, h", NULL, 0, nop, NULL, NULL},               // 0x64
    {"ld h, l", NULL, 0, ld_h_l, NULL, NULL},            // 0x65
    {"ld h, (hl)", NULL, 0, ld_h_hlp, NULL, NULL},       // 0x66
    {"ld h, a", NULL, 0, ld_h_a, NULL, NULL},            // 0x67
    {"ld l, b", NULL, 0, ld_l_b, NULL, NULL},            // 0x68
    {"ld l, c", NULL, 0, ld_l_c, NULL, NULL},            // 0x69
    {"ld l, d", NULL, 0, ld_l_d, NULL, NULL},            // 0x6a
    {"ld l, e", NULL, 0, ld_l_e, NULL, NULL},            // 0x6b
    {"ld l, h", NULL, 0, ld_l_h, NULL, NULL},            // 0x6c
    {"ld l, l", NULL, 0, nop, NULL, NULL},               // 0x6d
    {"ld l, (hl)", NULL, 0, ld_l_hlp, NULL, NULL},       // 0x6e
    {"ld l, a", NULL, 0, ld_l_a, NULL, NULL},            // 0x6f
    {"ld (hl), b", NULL, 0, ld_hlp_b, NULL, NULL},       // 0x70
    {"ld (hl), c", NULL, 0, ld_hlp_c, NULL, NULL},       // 0x71
    {"ld (hl), d", NULL, 0, ld_hlp_d, NULL, NULL},       // 0x72
    {"ld (hl), e", NULL, 0, ld_hlp_e, NULL, NULL},       // 0x73
    {"ld (hl), h", NULL, 0, ld_hlp_h, NULL, NULL},       // 0x74
    {"ld (hl), l", NULL, 0, ld_hlp_l, NULL, NULL},       // 0x75
    {"halt", NULL, 0, halt, NULL, NULL},                 // 0x76
    {"ld (hl), a", NULL, 0, ld_hlp_a, NULL, NULL},       // 0x77
    {"ld a, b", NULL, 0, ld_a_b, NULL, NULL},            // 0x78
    {"ld a, c", NULL, 0, ld_a_c, NULL, NULL},            // 0x79
    {"ld a, d", NULL, 0, ld_a_d, NULL, NULL},            // 0x7a
    {"ld a, e", NULL, 0, ld_a_e, NULL, NULL},            // 0x7b
    {"ld a, h", NULL, 0, ld_a_h, NULL, NULL},            // 0x7c
    {"ld a, l", NULL, 0, ld_a_l, NULL, NULL},            // 0x7d
    {"ld a, (hl)", NULL, 0, ld_a_hlp, NULL, NULL},       // 0x7e
    {"ld a, a", NULL, 0, nop, NULL, NULL},               // 0x7f
    {"add a, b", NULL, 0, add_a_b, NULL, NULL},          // 0x80
    {"add a, c", NULL, 0, add_a_c, NULL, NULL},          // 0x81
    {"add a, d", NULL, 0, add_a_d, NULL, NULL},          // 0x82
    {"add a, e", NULL, 0, add_a_e, NULL, NULL},          // 0x83
    {"add a, h", NULL, 0, add_a_h, NULL, NULL},          // 0x84
    {"add a, l", NULL, 0, add_a_l, NULL, NULL},          // 0x85
    {"add a, (hl)", NULL, 0, add_a_hlp, NULL, NULL},     // 0x86
    {"add a", NULL, 0, add_a_a, NULL, NULL},             // 0x87
    {"adc b", NULL, 0, adc_b, NULL, NULL},               // 0x88
    {"adc c", NULL, 0, adc_c, NULL, NULL},               // 0x89
    {"adc d", NULL, 0, adc_d, NULL, NULL},               // 0x8a
    {"adc e", NULL, 0, adc_e, NULL, NULL},               // 0x8b
    {"adc h", NULL, 0, adc_h, NULL, NULL},               // 0x8c
    {"adc l", NULL, 0, adc_l, NULL, NULL},               // 0x8d
    {"adc (hl)", NULL, 0, adc_hlp, NULL, NULL},          // 0x8e
    {"adc a", NULL, 0, adc_a, NULL, NULL},               // 0x8f
    {"sub b", NULL, 0, sub_b, NULL, NULL},               // 0x90
    {"sub c", NULL, 0, sub_c, NULL, NULL},               // 0x91
    {"sub d", NULL, 0, sub_d, NULL, NULL},               // 0x92
    {"sub e", NULL, 0, sub_e, NULL, NULL},               // 0x93
    {"sub h", NULL, 0, sub_h, NULL, NULL},               // 0x94
    {"sub l", NULL, 0, sub_l, NULL, NULL},               // 0x95
    {"sub (hl)", NULL, 0, sub_hlp, NULL, NULL},          // 0x96
    {"sub a", NULL, 0, sub_a, NULL, NULL},               // 0x97
    {"sbc b", NULL, 0, sbc_b, NULL, NULL},               // 0x98
    {"sbc c", NULL, 0, sbc_c, NULL, NULL},               // 0x99
    {"sbc d", NULL, 0, sbc_d, NULL, NULL},               // 0x9a
    {"sbc e", NULL, 0, sbc_e, NULL, NULL},               // 0x9b
    {"sbc h", NULL, 0, sbc_h, NULL, NULL},               // 0x9c
    {"sbc l", NULL, 0, sbc_l, NULL, NULL},               // 0x9d
    {"sbc (hl)", NULL, 0, sbc_hlp, NULL, NULL},          // 0x9e
    {"sbc a", NULL, 0, sbc_a, NULL, NULL},               // 0x9f
    {"and b", NULL, 0, and_b, NULL, NULL},               // 0xa0
    {"and c", NULL, 0, and_c, NULL, NULL},               // 0xa1
    {"and d", NULL, 0, and_d, NULL, NULL},               // 0xa2
    {"and e", NULL, 0, and_e, NULL, NULL},               // 0xa3
    {"and h", NULL, 0, and_h, NULL, NULL},               // 0xa4
    {"and l", NULL, 0, and_l, NULL, NULL},               // 0xa5
    {"and (hl)", NULL, 0, and_hlp, NULL, NULL},          // 0xa6
    {"and a", NULL, 0, and_a, NULL, NULL},               // 0xa7
    {"xor b", NULL, 0, xor_b, NULL, NULL},               // 0xa8
    {"xor c", NULL, 0, xor_c, NULL, NULL},               // 0xa9
    {"xor d", NULL, 0, xor_d, NULL, NULL},               // 0xaa
    {"xor e", NULL, 0, xor_e, NULL, NULL},               // 0xab
    {"xor h", NULL, 0, xor_h, NULL, NULL},               // 0xac
    {"xor l", NULL, 0, xor_l, NULL, NULL},               // 0xad
    {"xor (hl)", NULL, 0, xor_hlp, NULL, NULL},          // 0xae
    {"xor a", NULL, 0, xor_a, NULL, NULL},               // 0xaf
    {"or b", NULL, 0, or_b, NULL, NULL},                 // 0xb0
    {"or c", NULL, 0, or_c, NULL, NULL},                 // 0xb1
    {"or d", NULL, 0, or_d, NULL, NULL},                 // 0xb2
    {"or e", NULL, 0, or_e, NULL, NULL},                 // 0xb3
    {"or h", NULL, 0, or_h, NULL, NULL},                 // 0xb4
    {"or l", NULL, 0, or_l, NULL, NULL},                 // 0xb5
    {"or (hl)", NULL, 0, or_hlp, NULL, NULL},            // 0xb6
    {"or a", NULL, 0, or_a, NULL, NULL},                 // 0xb7
    {"cp b", NULL, 0, cp_b, NULL, NULL},                 // 0xb8
    {"cp c", NULL, 0, cp_c, NULL, NULL},                 // 0xb9
    {"cp d", NULL, 0, cp_d, NULL, NULL},                 // 0xba
    {"cp e", NULL, 0, cp_e, NULL, NULL},                 // 0xbb
    {"cp h", NULL, 0, cp_h, NULL, NULL},                 // 0xbc
    {"cp l", NULL, 0, cp_l, NULL, NULL},                 // 0xbd
    {"cp (hl)", NULL, 0, cp_hlp, NULL, NULL},            // 0xbe
    {"cp a", NULL, 0, cp_a, NULL, NULL},                 // 0xbf
    {"ret nz", NULL, 0, ret_nz, NULL, NULL},             // 0xc0
    {"pop bc", NULL, 0, pop_bc, NULL, NULL},             // 0xc1
    {"jp nz, $", NULL, 2, NULL, NULL, jp_nz_nn},         // 0xc2
    {"jp $", NULL, 2, NULL, NULL, jp_nn},                // 0xc3
    {"call nz, $", NULL, 2, NULL, NULL, call_nz_nn},     // 0xc4
    {"push bc", NULL, 0, push_bc, NULL, NULL},           // 0xc5
    {"add a, $", NULL, 1, NULL, add_a_n, NULL},          // 0xc6
    {"rst $00", NULL, 0, rst_00, NULL, NULL},            // 0xc7
    {"ret z", NULL, 0, ret_z, NULL, NULL},               // 0xc8
    {"ret", NULL, 0, ret, NULL, NULL},                   // 0xc9
    {"jp z, $", NULL, 2, NULL, NULL, jp_z_nn},           // 0xca
    {"cb ", NULL, 1, NULL, cb_n, NULL},                  // 0xcb
    {"call z, $", NULL, 2, NULL, NULL, call_z_nn},       // 0xcc
    {"call $", NULL, 2, NULL, NULL, call_nn},            // 0xcd
    {"adc $", NULL, 1, NULL, adc_n, NULL},               // 0xce
    {"rst $08", NULL, 0, rst_08, NULL, NULL},            // 0xcf
    {"ret nc", NULL, 0, ret_nc, NULL, NULL},             // 0xd0
    {"pop de", NULL, 0, pop_de, NULL, NULL},             // 0xd1
    {"jp nc, $", NULL, 2, NULL, NULL, jp_nc_nn},         // 0xd2
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xd3
    {"call nc, $", NULL, 2, NULL, NULL, call_nc_nn},     // 0xd4
    {"push de", NULL, 0, push_de, NULL, NULL},           // 0xd5
    {"sub $", NULL, 1, NULL, sub_n, NULL},               // 0xd6
    {"rst $10", NULL, 0, rst_10, NULL, NULL},            // 0xd7
    {"ret c", NULL, 0, ret_c, NULL, NULL},               // 0xd8
    {"reti", NULL, 0, reti, NULL, NULL},                 // 0xd9
    {"jp c, $", NULL, 2, NULL, NULL, jp_c_nn},           // 0xda
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xdb
    {"call c, $", NULL, 2, NULL, NULL, call_c_nn},       // 0xdc
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xdd
    {"sbc $", NULL, 1, NULL, sbc_n, NULL},               // 0xde
    {"rst $18", NULL, 0, rst_18, NULL, NULL},            // 0xdf
    {"ld ($ff00 + $", "), a", 1, NULL, ldh_n_a, NULL},   // 0xe0
    {"pop hl", NULL, 0, pop_hl, NULL, NULL},             // 0xe1
    {"ld ($ff00 + c), a", NULL, 0, ld_cp_a, NULL, NULL}, // 0xe2
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xe3
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xe4
    {"push hl", NULL, 0, push_hl, NULL, NULL},           // 0xe5
    {"and $", NULL, 1, NULL, and_n, NULL},               // 0xe6
    {"rst $20", NULL, 0, rst_20, NULL, NULL},            // 0xe7
    {"add sp,$", NULL, 1, NULL, add_sp_n, NULL},         // 0xe8
    {"jp hl", NULL, 0, jp_hl, NULL, NULL},               // 0xe9
    {"ld ($", "), a", 2, NULL, NULL, ld_nnp_a},          // 0xea
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xeb
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xec
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xed
    {"xor $", NULL, 1, NULL, xor_n, NULL},               // 0xee
    {"rst $28", NULL, 0, rst_28, NULL, NULL},            // 0xef
    {"ld a, ($ff00 + $", ")", 1, NULL, ldh_a_n, NULL},   // 0xf0
    {"pop af", NULL, 0, pop_af, NULL, NULL},             // 0xf1
    {"ld a, ($ff00 + c)", NULL, 0, ld_a_cp, NULL, NULL}, // 0xf2
    {"di", NULL, 0, di, NULL, NULL},                     // 0xf3
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xf4
    {"push af", NULL, 0, push_af, NULL, NULL},           // 0xf5
    {"or $", NULL, 1, NULL, or_n, NULL},                 // 0xf6
    {"rst $30", NULL, 0, rst_30, NULL, NULL},            // 0xf7
    {"ld hl, sp+$", NULL, 1, NULL, ldhl_sp_n, NULL},     // 0xf8
    {"ld sp, hl", NULL, 0, ld_sp_hl, NULL, NULL},        // 0xf9
    {"ld a, ($", ")", 2, NULL, NULL, ld_a_nnp},          // 0xfa
    {"ei", NULL, 0, ei, NULL, NULL},                     // 0xfb
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xfc
    {"unknown", NULL, 0, undefined, NULL, NULL},         // 0xfd
    {"cp $", NULL, 1, NULL, cp_n, NULL},                 // 0xfe
    {"rst $38", NULL, 0, rst_38, NULL, NULL},            // 0xff
};

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

void cpu_dump(void)
{
    printf("Dumping CPU info:\n");
    printf("PC:0x%04x SP:0x%04x\n", CPU.reg.pc, CPU.reg.sp);
    printf("AF:0x%04x BC:0x%04x DE:0x%04x HL:0x%04x\n", CPU.reg.af, CPU.reg.bc,
           CPU.reg.de, CPU.reg.hl);
    gpu_dump();
    mmu_dump(0xc000, 128);
    interrupt_dump();
}

int cpu_init(const char *rom_path)
{
    if (mmu_init(rom_path) < 0)
        return -1;
    cpu_reset();
    return 0;
}

void cpu_finish(void)
{
    mmu_finish();
}

void cpu_reset(void)
{
    memset(&CPU, 0x0, sizeof(CPU));
    CPU.reg.pc = 0x0100;
    CPU.reg.sp = 0xfffe;
    if (cart_is_cgb()) {
        CPU.reg.af = 0x1180;
        CPU.reg.bc = 0x0000;
        CPU.reg.de = 0xff56;
        CPU.reg.hl = 0x000d;
    } else {
        CPU.reg.af = 0x01b0;
        CPU.reg.bc = 0x0013;
        CPU.reg.de = 0x00d8;
        CPU.reg.hl = 0x014d;
    }
    clock_reset();
    mmu_reset();
}

static uint8_t cpu_fetch_opcode(void)
{
    uint8_t op = mmu_read_byte(CPU.reg.pc);
    CPU.last_pc = CPU.reg.pc++;
    return op;
}

static void cpu_decode_opcode(uint8_t opcode)
{
    uint8_t oper_length = g_instr[opcode].operand_length;
    printd("PC:0x%04x SP:0x%04x AF:0x%04x BC:0x%04x DE:0x%04x HL:0x%04x: ",
           CPU.last_pc, CPU.reg.sp, CPU.reg.af, CPU.reg.bc, CPU.reg.de,
           CPU.reg.hl);
    if (oper_length == 0) {
        printd("%s\n", cpu_debug_instr0(debug_str, opcode));
        g_instr[opcode].exec0();
    } else if (oper_length == 1) {
        uint8_t operand = mmu_read_byte(CPU.reg.pc);
        CPU.reg.pc = (uint16_t)(CPU.reg.pc + 1);
        printd("%s\n", cpu_debug_instr1(debug_str, opcode, operand));
        g_instr[opcode].exec1(operand);
    } else if (oper_length == 2) {
        uint16_t operand = mmu_read_word(CPU.reg.pc);
        CPU.reg.pc = (uint16_t)(CPU.reg.pc + 2);
        printd("%s\n", cpu_debug_instr2(debug_str, opcode, operand));
        g_instr[opcode].exec2(operand);
    } else {
        error("invalid operand length %hhu", oper_length);
    }
}

void cpu_emulate_cycle(void)
{
    interrupt_step();
    clock_clear();
    if (CPU.halt) {
        /* Tick clock while halted. */
        clock_step(4);
    } else {
        uint8_t opcode = cpu_fetch_opcode();
        cpu_decode_opcode(opcode);
    }
    gpu_tick(clock_get_step());
}
