#include "op_enc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *output;
unsigned int pc;

#define error(fmt, ...)                                      \
    {                                                        \
        fprintf(stderr, "parse error: " fmt, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);                                  \
    }

static unsigned int get_reg_diff(reg_t reg)
{
    switch (reg) {
        case REG_B:
            return 0;
        case REG_C:
            return 1;
        case REG_D:
            return 2;
        case REG_E:
            return 3;
        case REG_H:
            return 4;
        case REG_L:
            return 5;
        case REG_HL:
            return 6;
        case REG_A:
            return 7;
        default:
            abort();
    }
}

static inline void op_write1(uint8_t opcode)
{
    fwrite(&opcode, 1, 1, output);
    pc += 1;
}

static inline void op_write2(uint8_t opcode, uint8_t val)
{
    uint8_t buf[2];
    buf[0] = opcode;
    buf[1] = val;
    fwrite(buf, 2, 1, output);
    pc += 2;
}

static inline void op_write3(uint8_t opcode, uint16_t val)
{
    uint8_t buf[3];
    buf[0] = opcode;
    buf[1] = (uint8_t)(val & 0x00ff);
    buf[2] = (uint8_t)((val & 0xff00) >> 8);
    fwrite(buf, 3, 1, output);
    pc += 3;
}

static inline void op_write_cb(uint8_t opcode)
{
    uint8_t buf[2];
    buf[0] = 0xcb;
    buf[1] = opcode;
    fwrite(buf, 2, 1, output);
    pc += 2;
}

void op_enc_init(FILE *out)
{
    pc = 0;
    output = out;
}

unsigned int op_enc_get_pc(void)
{
    return pc;
}

void op_enc_jp_addr(unsigned int addr, uint16_t jump_addr)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(jump_addr & 0x00ff);
    buf[1] = (uint8_t)((jump_addr & 0xff00) >> 8);
    fseek(output, addr + 1, SEEK_SET);
    fwrite(buf, 2, 1, output);
}

void op_enc_jr_addr(unsigned int addr, unsigned int jump_addr)
{
    int8_t val = jump_addr - (addr + 2);
    fseek(output, addr + 1, SEEK_SET);
    fwrite(&val, 1, 1, output);
}

void op_enc_db(uint8_t val)
{
    op_write1(val);
}

void op_enc_db_ascii(char *str)
{
    size_t size = strlen(str) - 1;
    for (size_t i = 0; i <= size; ++i) {
        if (str[i] == '\\') {
            switch (str[i + 1]) {
                case '0':
                    op_write1(0x00);
                    ++i;
                    continue;
                default:
                    error("warning:ascii: unimplemented \\%c\n", str[i + 1]);
            }
        }
        op_write1(str[i]);
    }
}

void op_enc_org(long offset)
{
    pc = offset;
    int ret = fseek(output, offset, SEEK_SET);
    if (ret != 0) {
        error("ERROR: org: invalid position: %ld\n", offset);
    }
}

void op_enc_fill(uint8_t c, size_t n)
{
    n &= 0xffffff;
    for (size_t i = 0; i < n; ++i) {
        op_write1(c);
    }
}

void op_enc_nop(void)
{
    op_write1(0x00);
}

void op_enc_ld_bc_nn(uint16_t val)
{
    op_write3(0x01, val);
}

void op_enc_ld_bcp_a(void)
{
    op_write1(0x02);
}

void op_enc_inc_bc(void)
{
    op_write1(0x03);
}

void op_enc_inc_b(void)
{
    op_write1(0x04);
}

void op_enc_dec_b(void)
{
    op_write1(0x05);
}

void op_enc_ld_b_n(uint8_t val)
{
    op_write2(0x06, val);
}

void op_enc_rlca(void)
{
    op_write1(0x07);
}

void op_enc_ld_nnp_sp(uint16_t val)
{
    op_write3(0x08, val);
}

void op_enc_add_hl_bc(void)
{
    op_write1(0x09);
}

void op_enc_ld_a_bcp(void)
{
    op_write1(0x0a);
}

void op_enc_dec_bc(void)
{
    op_write1(0x0b);
}

void op_enc_inc_c(void)
{
    op_write1(0x0c);
}

void op_enc_dec_c(void)
{
    op_write1(0x0d);
}

void op_enc_ld_c_n(uint8_t val)
{
    op_write2(0x0e, val);
}

void op_enc_rrca(void)
{
    op_write1(0x0f);
}

void op_enc_stop(void)
{
    op_write1(0x10);
}

void op_enc_ld_de_nn(uint16_t val)
{
    op_write3(0x11, val);
}

void op_enc_ld_dep_a(void)
{
    op_write1(0x12);
}

void op_enc_inc_de(void)
{
    op_write1(0x13);
}

void op_enc_inc_d(void)
{
    op_write1(0x14);
}

void op_enc_dec_d(void)
{
    op_write1(0x15);
}

void op_enc_ld_d_n(uint8_t val)
{
    op_write2(0x16, val);
}

void op_enc_rla(void)
{
    op_write1(0x17);
}

void op_enc_jr_n(uint8_t val)
{
    op_write2(0x18, val);
}

void op_enc_add_hl_de(void)
{
    op_write1(0x19);
}

void op_enc_ld_a_dep(void)
{
    op_write1(0x1a);
}

void op_enc_dec_de(void)
{
    op_write1(0x1b);
}

void op_enc_inc_e(void)
{
    op_write1(0x1c);
}

void op_enc_dec_e(void)
{
    op_write1(0x1d);
}

void op_enc_ld_e_n(uint8_t val)
{
    op_write2(0x1e, val);
}

void op_enc_rra(void)
{
    op_write1(0x1f);
}

void op_enc_jr_nz_n(int8_t val)
{
    op_write2(0x20, val);
}

void op_enc_ld_hl_nn(uint16_t val)
{
    op_write3(0x21, val);
}

void op_enc_ldi_hlp_a(void)
{
    op_write1(0x22);
}

void op_enc_inc_hl(void)
{
    op_write1(0x23);
}

void op_enc_inc_h(void)
{
    op_write1(0x24);
}

void op_enc_dec_h(void)
{
    op_write1(0x25);
}

void op_enc_ld_h_n(uint8_t val)
{
    op_write2(0x26, val);
}

void op_enc_daa(void)
{
    op_write1(0x27);
}

void op_enc_jr_z_n(uint8_t val)
{
    op_write2(0x28, val);
}

void op_enc_add_hl_hl(void)
{
    op_write1(0x29);
}

void op_enc_ldi_a_hlp(void)
{
    op_write1(0x2a);
}

void op_enc_dec_hl(void)
{
    op_write1(0x2b);
}

void op_enc_inc_l(void)
{
    op_write1(0x2c);
}

void op_enc_dec_l(void)
{
    op_write1(0x2d);
}

void op_enc_ld_l_n(uint8_t val)
{
    op_write2(0x2e, val);
}

void op_enc_cpl(void)
{
    op_write1(0x2f);
}

void op_enc_jr_nc_n(uint8_t val)
{
    op_write2(0x30, val);
}

void op_enc_ld_sp_nn(uint16_t val)
{
    op_write3(0x31, val);
}

void op_enc_ldd_hlp_a(void)
{
    op_write1(0x32);
}

void op_enc_inc_sp(void)
{
    op_write1(0x33);
}

void op_enc_inc_hlp(void)
{
    op_write1(0x34);
}

void op_enc_dec_hlp(void)
{
    op_write1(0x35);
}

void op_enc_ld_hlp_n(uint8_t val)
{
    op_write2(0x36, val);
}

void op_enc_scf(void)
{
    op_write1(0x37);
}

void op_enc_jr_c_n(uint8_t val)
{
    op_write2(0x38, val);
}

void op_enc_add_hl_sp(void)
{
    op_write1(0x39);
}

void op_enc_ldd_a_hlp(void)
{
    op_write1(0x3a);
}

void op_enc_dec_sp(void)
{
    op_write1(0x3b);
}

void op_enc_inc_a(void)
{
    op_write1(0x3c);
}

void op_enc_dec_a(void)
{
    op_write1(0x3d);
}

void op_enc_ld_a_n(uint8_t val)
{
    op_write2(0x3e, val);
}

void op_enc_ccf(void)
{
    op_write1(0x3f);
}

void op_enc_ld_b_reg(reg_t reg)
{
    unsigned int opcode = 0x40 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_c_reg(reg_t reg)
{
    unsigned int opcode = 0x48 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_d_reg(reg_t reg)
{
    unsigned int opcode = 0x50 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_e_reg(reg_t reg)
{
    unsigned int opcode = 0x58 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_h_reg(reg_t reg)
{
    unsigned int opcode = 0x60 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_l_reg(reg_t reg)
{
    unsigned int opcode = 0x68 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ld_hlp_reg(reg_t reg)
{
    unsigned int opcode = 0x70;
    switch (reg) {
        case REG_B:
            opcode += 0;
            break;
        case REG_C:
            opcode += 1;
            break;
        case REG_D:
            opcode += 2;
            break;
        case REG_E:
            opcode += 3;
            break;
        case REG_H:
            opcode += 4;
            break;
        case REG_L:
            opcode += 5;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            abort();
    }
    op_write1((uint8_t)opcode);
}

void op_enc_halt(void)
{
    op_write1(0x76);
}

void op_enc_ld_a_reg(reg_t reg)
{
    unsigned int opcode = 0x78 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_add_a_reg(reg_t reg)
{
    unsigned int opcode = 0x80 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_adc(reg_t reg)
{
    unsigned int opcode = 0x88 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_sub(reg_t reg)
{
    unsigned int opcode = 0x90 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_sbc(reg_t reg)
{
    unsigned int opcode = 0x98 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_and(reg_t reg)
{
    unsigned int opcode = 0xa0 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_xor(reg_t reg)
{
    unsigned int opcode = 0xa8 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_or(reg_t reg)
{
    unsigned int opcode = 0xb0 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_cp(reg_t reg)
{
    unsigned int opcode = 0xb8 + get_reg_diff(reg);
    op_write1((uint8_t)opcode);
}

void op_enc_ret_nz(void)
{
    op_write1(0xc0);
}

void op_enc_pop_bc(void)
{
    op_write1(0xc1);
}

void op_enc_jp_nz_nn(uint16_t val)
{
    op_write3(0xc2, val);
}

void op_enc_jp_nn(uint16_t val)
{
    op_write3(0xc3, val);
}

void op_enc_call_nz_nn(uint16_t val)
{
    op_write3(0xc4, val);
}

void op_enc_push_bc(void)
{
    op_write1(0xc5);
}

void op_enc_add_a_n(uint8_t val)
{
    op_write2(0xc6, val);
}

void op_enc_rst(uint8_t val)
{
    if (val == 0x00) {
        op_write1(0xc7);
    } else if (val == 0x08) {
        op_write1(0xcf);
    } else if (val == 0x10) {
        op_write1(0xd7);
    } else if (val == 0x18) {
        op_write1(0xdf);
    } else if (val == 0x20) {
        op_write1(0xe7);
    } else if (val == 0x28) {
        op_write1(0xef);
    } else if (val == 0x30) {
        op_write1(0xf7);
    } else if (val == 0x38) {
        op_write1(0xff);
    }
}

void op_enc_ret_z(void)
{
    op_write1(0xc8);
}

void op_enc_ret(void)
{
    op_write1(0xc9);
}

void op_enc_jp_z_nn(uint16_t val)
{
    op_write3(0xca, val);
}

void op_enc_cb_n(uint8_t val)
{
    op_write2(0xcb, val);
}

void op_enc_call_z_nn(uint16_t val)
{
    op_write3(0xcc, val);
}

void op_enc_call_nn(uint16_t val)
{
    op_write3(0xcd, val);
}

void op_enc_adc_n(uint8_t val)
{
    op_write2(0xce, val);
}

void op_enc_ret_nc(void)
{
    op_write1(0xd0);
}

void op_enc_pop_de(void)
{
    op_write1(0xd1);
}

void op_enc_jp_nc_nn(uint16_t val)
{
    op_write3(0xd2, val);
}

void op_enc_call_nc_nn(uint16_t val)
{
    op_write3(0xd4, val);
}

void op_enc_push_de(void)
{
    op_write1(0xd5);
}

void op_enc_sub_n(uint8_t val)
{
    op_write2(0xd6, val);
}

void op_enc_ret_c(void)
{
    op_write1(0xd8);
}

void op_enc_reti(void)
{
    op_write1(0xd9);
}

void op_enc_jp_c_nn(uint16_t val)
{
    op_write3(0xda, val);
}

void op_enc_call_c_nn(uint16_t val)
{
    op_write3(0xdc, val);
}

void op_enc_sbc_n(uint8_t val)
{
    op_write2(0xde, val);
}

void op_enc_ldh_n_a(uint8_t val)
{
    op_write2(0xe0, val);
}

void op_enc_pop_hl(void)
{
    op_write1(0xe1);
}

void op_enc_ld_cp_a(void)
{
    op_write1(0xe2);
}

void op_enc_push_hl(void)
{
    op_write1(0xe5);
}

void op_enc_and_n(uint8_t val)
{
    op_write2(0xe6, val);
}

void op_enc_add_sp_n(uint8_t val)
{
    op_write2(0xe8, val);
}

void op_enc_jp_hl(void)
{
    op_write1(0xe9);
}

void op_enc_ld_nnp_a(uint16_t val)
{
    op_write3(0xea, val);
}

void op_enc_xor_n(uint8_t val)
{
    op_write2(0xee, val);
}

void op_enc_ldh_a_n(uint8_t val)
{
    op_write2(0xf0, val);
}

void op_enc_pop_af(void)
{
    op_write1(0xf1);
}

void op_enc_ld_a_cp(void)
{
    op_write1(0xf2);
}

void op_enc_di(void)
{
    op_write1(0xf3);
}

void op_enc_push_af(void)
{
    op_write1(0xf5);
}

void op_enc_or_n(uint8_t val)
{
    op_write2(0xf6, val);
}

void op_enc_ldhl_sp_n(uint8_t val)
{
    op_write2(0xf8, val);
}

void op_enc_ld_sp_hl(void)
{
    op_write1(0xf9);
}

void op_enc_ld_a_nnp(uint16_t val)
{
    op_write3(0xfa, val);
}

void op_enc_ei(void)
{
    op_write1(0xfb);
}

void op_enc_cp_n(uint8_t val)
{
    op_write2(0xfe, val);
}

/* CB */

void op_enc_rlc(reg_t reg)
{
    unsigned int opcode = 0x00 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_rrc(reg_t reg)
{
    unsigned int opcode = 0x08 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_rl(reg_t reg)
{
    unsigned int opcode = 0x10 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_rr(reg_t reg)
{
    unsigned int opcode = 0x18 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_sla(reg_t reg)
{
    unsigned int opcode = 0x20 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_sra(reg_t reg)
{
    unsigned int opcode = 0x28 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_swap(reg_t reg)
{
    unsigned int opcode = 0x30 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_srl(reg_t reg)
{
    unsigned int opcode = 0x38 + get_reg_diff(reg);
    op_write_cb(opcode);
}

void op_enc_bit(unsigned int b, reg_t reg)
{
    unsigned int opcode = 0x40u + get_reg_diff(reg) + 8u * b;
    op_write_cb((uint8_t)opcode);
}

void op_enc_res(unsigned int b, reg_t reg)
{
    unsigned int opcode = 0x80u + get_reg_diff(reg) + 8u * b;
    op_write_cb((uint8_t)opcode);
}

void op_enc_set(unsigned int b, reg_t reg)
{
    unsigned int opcode = 0xc0u + get_reg_diff(reg) + 8u * b;
    op_write_cb((uint8_t)opcode);
}
