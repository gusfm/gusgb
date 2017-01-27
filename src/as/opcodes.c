#include "opcodes.h"
#include <stdio.h>
#include <stdlib.h>

extern unsigned int linenum;

const char *regs[REG_LEN] = {"a", "f", "b", "c", "d", "e", "h", "l", "af", "bc", "de", "hl"};

void error_register(register_e reg)
{
    fprintf(stderr, "%u: invalid register: %s\n", linenum, regs[reg]);
    exit(EXIT_FAILURE);
}

void error_bit(unsigned int bit)
{
    fprintf(stderr, "%u: invalid bit: %u\n", linenum, bit);
    exit(EXIT_FAILURE);
}

static inline void op_write1(FILE *f, uint8_t opcode)
{
    fwrite(&opcode, 1, 1, f);
}

static inline void op_write2(FILE *f, uint8_t opcode, uint8_t val)
{
    uint8_t buf[2];
    buf[0] = opcode;
    buf[1] = val;
    fwrite(buf, 2, 1, f);
}

static inline void op_write3(FILE *f, uint8_t opcode, uint16_t val)
{
    uint8_t buf[3];
    buf[0] = opcode;
    buf[1] = (uint8_t)(val & 0x00ff);
    buf[2] = (uint8_t)((val & 0xff00) >> 8);
    fwrite(buf, 3, 1, f);
}

static inline void op_write_cb(FILE *f, uint8_t opcode)
{
    uint8_t buf[2];
    buf[0] = 0xcb;
    buf[1] = opcode;
    fwrite(buf, 2, 1, f);
}

void data(FILE *f, uint8_t val)
{
    op_write1(f, val);
}

void nop(FILE *f)
{
    op_write1(f, 0x00);
}

void ld_bc_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0x01, val);
}

void ld_bcp_a(FILE *f)
{
    op_write1(f, 0x02);
}

void inc_bc(FILE *f)
{
    op_write1(f, 0x03);
}

void inc_b(FILE *f)
{
    op_write1(f, 0x04);
}

void dec_b(FILE *f)
{
    op_write1(f, 0x05);
}

void ld_b_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x06, val);
}

void rlca(FILE *f)
{
    op_write1(f, 0x07);
}

void ld_nnp_sp(FILE *f, uint16_t val)
{
    op_write3(f, 0x08, val);
}

void add_hl_bc(FILE *f)
{
    op_write1(f, 0x09);
}

void ld_a_bcp(FILE *f)
{
    op_write1(f, 0x0a);
}

void dec_bc(FILE *f)
{
    op_write1(f, 0x0b);
}

void inc_c(FILE *f)
{
    op_write1(f, 0x0c);
}

void dec_c(FILE *f)
{
    op_write1(f, 0x0d);
}

void ld_c_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x0e, val);
}

void rrca(FILE *f)
{
    op_write1(f, 0x0f);
}

void stop(FILE *f)
{
    op_write1(f, 0x10);
}

void ld_de_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0x11, val);
}

void ld_dep_a(FILE *f)
{
    op_write1(f, 0x12);
}

void inc_de(FILE *f)
{
    op_write1(f, 0x13);
}

void inc_d(FILE *f)
{
    op_write1(f, 0x14);
}

void dec_d(FILE *f)
{
    op_write1(f, 0x15);
}

void ld_d_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x16, val);
}

void rla(FILE *f)
{
    op_write1(f, 0x17);
}

void jr_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x18, val);
}

void add_hl_de(FILE *f)
{
    op_write1(f, 0x19);
}

void ld_a_dep(FILE *f)
{
    op_write1(f, 0x1a);
}

void dec_de(FILE *f)
{
    op_write1(f, 0x1b);
}

void inc_e(FILE *f)
{
    op_write1(f, 0x1c);
}

void dec_e(FILE *f)
{
    op_write1(f, 0x1d);
}

void ld_e_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x1e, val);
}

void rra(FILE *f)
{
    op_write1(f, 0x1f);
}

void jr_nz_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x20, val);
}

void ld_hl_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0x21, val);
}

void ldi_hlp_a(FILE *f)
{
    op_write1(f, 0x22);
}

void inc_hl(FILE *f)
{
    op_write1(f, 0x23);
}

void inc_h(FILE *f)
{
    op_write1(f, 0x24);
}

void dec_h(FILE *f)
{
    op_write1(f, 0x25);
}

void ld_h_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x26, val);
}

void daa(FILE *f)
{
    op_write1(f, 0x27);
}

void jr_z_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x28, val);
}

void add_hl_hl(FILE *f)
{
    op_write1(f, 0x29);
}

void ldi_a_hlp(FILE *f)
{
    op_write1(f, 0x2a);
}

void dec_hl(FILE *f)
{
    op_write1(f, 0x2b);
}

void inc_l(FILE *f)
{
    op_write1(f, 0x2c);
}

void dec_l(FILE *f)
{
    op_write1(f, 0x2d);
}

void ld_l_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x2e, val);
}

void cpl(FILE *f)
{
    op_write1(f, 0x2f);
}

void jr_nc_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x30, val);
}

void ld_sp_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0x31, val);
}

void ldd_hlp_a(FILE *f)
{
    op_write1(f, 0x32);
}

void inc_sp(FILE *f)
{
    op_write1(f, 0x33);
}

void inc_hlp(FILE *f)
{
    op_write1(f, 0x34);
}

void dec_hlp(FILE *f)
{
    op_write1(f, 0x35);
}

void ld_hlp_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x36, val);
}

void scf(FILE *f)
{
    op_write1(f, 0x37);
}

void jr_c_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x38, val);
}

void add_hl_sp(FILE *f)
{
    op_write1(f, 0x39);
}

void ldd_a_hlp(FILE *f)
{
    op_write1(f, 0x3a);
}

void dec_sp(FILE *f)
{
    op_write1(f, 0x3b);
}

void inc_a(FILE *f)
{
    op_write1(f, 0x3c);
}

void dec_a(FILE *f)
{
    op_write1(f, 0x3d);
}

void ld_a_n(FILE *f, uint8_t val)
{
    op_write2(f, 0x3e, val);
}

void ccf(FILE *f)
{
    op_write1(f, 0x3f);
}

void ld_b_b(FILE *f)
{
    op_write1(f, 0x40);
}

void ld_b_c(FILE *f)
{
    op_write1(f, 0x41);
}

void ld_b_d(FILE *f)
{
    op_write1(f, 0x42);
}

void ld_b_e(FILE *f)
{
    op_write1(f, 0x43);
}

void ld_b_h(FILE *f)
{
    op_write1(f, 0x44);
}

void ld_b_l(FILE *f)
{
    op_write1(f, 0x45);
}

void ld_b_hlp(FILE *f)
{
    op_write1(f, 0x46);
}

void ld_b_a(FILE *f)
{
    op_write1(f, 0x47);
}

void ld_c_b(FILE *f)
{
    op_write1(f, 0x48);
}

void ld_c_c(FILE *f)
{
    op_write1(f, 0x49);
}

void ld_c_d(FILE *f)
{
    op_write1(f, 0x4a);
}

void ld_c_e(FILE *f)
{
    op_write1(f, 0x4b);
}

void ld_c_h(FILE *f)
{
    op_write1(f, 0x4c);
}

void ld_c_l(FILE *f)
{
    op_write1(f, 0x4d);
}

void ld_c_hlp(FILE *f)
{
    op_write1(f, 0x4e);
}

void ld_c_a(FILE *f)
{
    op_write1(f, 0x4f);
}

void ld_d_b(FILE *f)
{
    op_write1(f, 0x50);
}

void ld_d_c(FILE *f)
{
    op_write1(f, 0x51);
}

void ld_d_d(FILE *f)
{
    op_write1(f, 0x52);
}

void ld_d_e(FILE *f)
{
    op_write1(f, 0x53);
}

void ld_d_h(FILE *f)
{
    op_write1(f, 0x54);
}

void ld_d_l(FILE *f)
{
    op_write1(f, 0x55);
}

void ld_d_hlp(FILE *f)
{
    op_write1(f, 0x56);
}

void ld_d_a(FILE *f)
{
    op_write1(f, 0x57);
}

void ld_e_b(FILE *f)
{
    op_write1(f, 0x58);
}

void ld_e_c(FILE *f)
{
    op_write1(f, 0x59);
}

void ld_e_d(FILE *f)
{
    op_write1(f, 0x5a);
}

void ld_e_e(FILE *f)
{
    op_write1(f, 0x5b);
}

void ld_e_h(FILE *f)
{
    op_write1(f, 0x5c);
}

void ld_e_l(FILE *f)
{
    op_write1(f, 0x5d);
}

void ld_e_hlp(FILE *f)
{
    op_write1(f, 0x5e);
}

void ld_e_a(FILE *f)
{
    op_write1(f, 0x5f);
}

void ld_h_b(FILE *f)
{
    op_write1(f, 0x60);
}

void ld_h_c(FILE *f)
{
    op_write1(f, 0x61);
}

void ld_h_d(FILE *f)
{
    op_write1(f, 0x62);
}

void ld_h_e(FILE *f)
{
    op_write1(f, 0x63);
}

void ld_h_h(FILE *f)
{
    op_write1(f, 0x64);
}

void ld_h_l(FILE *f)
{
    op_write1(f, 0x65);
}

void ld_h_hlp(FILE *f)
{
    op_write1(f, 0x66);
}

void ld_h_a(FILE *f)
{
    op_write1(f, 0x67);
}

void ld_l_b(FILE *f)
{
    op_write1(f, 0x68);
}

void ld_l_c(FILE *f)
{
    op_write1(f, 0x69);
}

void ld_l_d(FILE *f)
{
    op_write1(f, 0x6a);
}

void ld_l_e(FILE *f)
{
    op_write1(f, 0x6b);
}

void ld_l_h(FILE *f)
{
    op_write1(f, 0x6c);
}

void ld_l_l(FILE *f)
{
    op_write1(f, 0x6d);
}

void ld_l_hlp(FILE *f)
{
    op_write1(f, 0x6e);
}

void ld_l_a(FILE *f)
{
    op_write1(f, 0x6f);
}

void ld_hlp_b(FILE *f)
{
    op_write1(f, 0x70);
}

void ld_hlp_c(FILE *f)
{
    op_write1(f, 0x71);
}

void ld_hlp_d(FILE *f)
{
    op_write1(f, 0x72);
}

void ld_hlp_e(FILE *f)
{
    op_write1(f, 0x73);
}

void ld_hlp_h(FILE *f)
{
    op_write1(f, 0x74);
}

void ld_hlp_l(FILE *f)
{
    op_write1(f, 0x75);
}

void halt(FILE *f)
{
    op_write1(f, 0x76);
}

void ld_hlp_a(FILE *f)
{
    op_write1(f, 0x77);
}

void ld_a_b(FILE *f)
{
    op_write1(f, 0x78);
}

void ld_a_c(FILE *f)
{
    op_write1(f, 0x79);
}

void ld_a_d(FILE *f)
{
    op_write1(f, 0x7a);
}

void ld_a_e(FILE *f)
{
    op_write1(f, 0x7b);
}

void ld_a_h(FILE *f)
{
    op_write1(f, 0x7c);
}

void ld_a_l(FILE *f)
{
    op_write1(f, 0x7d);
}

void ld_a_hlp(FILE *f)
{
    op_write1(f, 0x7e);
}

void ld_a_a(FILE *f)
{
    op_write1(f, 0x7f);
}

void add_a_b(FILE *f)
{
    op_write1(f, 0x80);
}

void add_a_c(FILE *f)
{
    op_write1(f, 0x81);
}

void add_a_d(FILE *f)
{
    op_write1(f, 0x82);
}

void add_a_e(FILE *f)
{
    op_write1(f, 0x83);
}

void add_a_h(FILE *f)
{
    op_write1(f, 0x84);
}

void add_a_l(FILE *f)
{
    op_write1(f, 0x85);
}

void add_a_hlp(FILE *f)
{
    op_write1(f, 0x86);
}

void add_a_a(FILE *f)
{
    op_write1(f, 0x87);
}

void adc_b(FILE *f)
{
    op_write1(f, 0x88);
}

void adc_c(FILE *f)
{
    op_write1(f, 0x89);
}

void adc_d(FILE *f)
{
    op_write1(f, 0x8a);
}

void adc_e(FILE *f)
{
    op_write1(f, 0x8b);
}

void adc_h(FILE *f)
{
    op_write1(f, 0x8c);
}

void adc_l(FILE *f)
{
    op_write1(f, 0x8d);
}

void adc_hlp(FILE *f)
{
    op_write1(f, 0x8e);
}

void adc_a(FILE *f)
{
    op_write1(f, 0x8f);
}

void sub_b(FILE *f)
{
    op_write1(f, 0x90);
}

void sub_c(FILE *f)
{
    op_write1(f, 0x91);
}

void sub_d(FILE *f)
{
    op_write1(f, 0x92);
}

void sub_e(FILE *f)
{
    op_write1(f, 0x93);
}

void sub_h(FILE *f)
{
    op_write1(f, 0x94);
}

void sub_l(FILE *f)
{
    op_write1(f, 0x95);
}

void sub_hlp(FILE *f)
{
    op_write1(f, 0x96);
}

void sub_a(FILE *f)
{
    op_write1(f, 0x97);
}

void sbc_b(FILE *f)
{
    op_write1(f, 0x98);
}

void sbc_c(FILE *f)
{
    op_write1(f, 0x99);
}

void sbc_d(FILE *f)
{
    op_write1(f, 0x9a);
}

void sbc_e(FILE *f)
{
    op_write1(f, 0x9b);
}

void sbc_h(FILE *f)
{
    op_write1(f, 0x9c);
}

void sbc_l(FILE *f)
{
    op_write1(f, 0x9d);
}

void sbc_hlp(FILE *f)
{
    op_write1(f, 0x9e);
}

void sbc_a(FILE *f)
{
    op_write1(f, 0x9f);
}

void and_b(FILE *f)
{
    op_write1(f, 0xa0);
}

void and_c(FILE *f)
{
    op_write1(f, 0xa1);
}

void and_d(FILE *f)
{
    op_write1(f, 0xa2);
}

void and_e(FILE *f)
{
    op_write1(f, 0xa3);
}

void and_h(FILE *f)
{
    op_write1(f, 0xa4);
}

void and_l(FILE *f)
{
    op_write1(f, 0xa5);
}

void and_hlp(FILE *f)
{
    op_write1(f, 0xa6);
}

void and_a(FILE *f)
{
    op_write1(f, 0xa7);
}

void xorf(FILE *f, register_e reg)
{
    unsigned int opcode = 0xa8;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write1(f, (uint8_t)opcode);
}

void or_b(FILE *f)
{
    op_write1(f, 0xb0);
}

void or_c(FILE *f)
{
    op_write1(f, 0xb1);
}

void or_d(FILE *f)
{
    op_write1(f, 0xb2);
}

void or_e(FILE *f)
{
    op_write1(f, 0xb3);
}

void or_h(FILE *f)
{
    op_write1(f, 0xb4);
}

void or_l(FILE *f)
{
    op_write1(f, 0xb5);
}

void or_hlp(FILE *f)
{
    op_write1(f, 0xb6);
}

void or_a(FILE *f)
{
    op_write1(f, 0xb7);
}

void cp_b(FILE *f)
{
    op_write1(f, 0xb8);
}

void cp_c(FILE *f)
{
    op_write1(f, 0xb9);
}

void cp_d(FILE *f)
{
    op_write1(f, 0xba);
}

void cp_e(FILE *f)
{
    op_write1(f, 0xbb);
}

void cp_h(FILE *f)
{
    op_write1(f, 0xbc);
}

void cp_l(FILE *f)
{
    op_write1(f, 0xbd);
}

void cp_hlp(FILE *f)
{
    op_write1(f, 0xbe);
}

void cp_a(FILE *f)
{
    op_write1(f, 0xbf);
}

void ret_nz(FILE *f)
{
    op_write1(f, 0xc0);
}

void pop_bc(FILE *f)
{
    op_write1(f, 0xc1);
}

void jp_nz_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xc2, val);
}

void jp_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xc3, val);
}

void call_nz_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xc4, val);
}

void push_bc(FILE *f)
{
    op_write1(f, 0xc5);
}

void add_a_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xc6, val);
}

void rst(FILE *f, uint8_t val)
{
    if (val == 0x00) {
        op_write1(f, 0xc7);
    } else if (val == 0x08) {
        op_write1(f, 0xcf);
    } else if (val == 0x10) {
        op_write1(f, 0xd7);
    } else if (val == 0x18) {
        op_write1(f, 0xdf);
    } else if (val == 0x20) {
        op_write1(f, 0xe7);
    } else if (val == 0x28) {
        op_write1(f, 0xef);
    } else if (val == 0x30) {
        op_write1(f, 0xf7);
    } else if (val == 0x38) {
        op_write1(f, 0xff);
    } else {
        fprintf(stderr, "rst: invalid value: %hhu\n", val);
        exit(EXIT_FAILURE);
    }
}

void ret_z(FILE *f)
{
    op_write1(f, 0xc8);
}

void ret(FILE *f)
{
    op_write1(f, 0xc9);
}

void jp_z_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xca, val);
}

void cb_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xcb, val);
}

void call_z_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xcc, val);
}

void call_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xcd, val);
}

void adc_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xce, val);
}

void ret_nc(FILE *f)
{
    op_write1(f, 0xd0);
}

void pop_de(FILE *f)
{
    op_write1(f, 0xd1);
}

void jp_nc_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xd2, val);
}

void call_nc_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xd4, val);
}

void push_de(FILE *f)
{
    op_write1(f, 0xd5);
}

void sub_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xd6, val);
}

void ret_c(FILE *f)
{
    op_write1(f, 0xd8);
}

void reti(FILE *f)
{
    op_write1(f, 0xd9);
}

void jp_c_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xda, val);
}

void call_c_nn(FILE *f, uint16_t val)
{
    op_write3(f, 0xdc, val);
}

void sbc_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xde, val);
}

void ldh_n_a(FILE *f, uint8_t val)
{
    op_write2(f, 0xe0, val);
}

void pop_hl(FILE *f)
{
    op_write1(f, 0xe1);
}

void ld_cp_a(FILE *f)
{
    op_write1(f, 0xe2);
}

void push_hl(FILE *f)
{
    op_write1(f, 0xe5);
}

void and_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xe6, val);
}

void add_sp_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xe8, val);
}

void jp_hl(FILE *f)
{
    op_write1(f, 0xe9);
}

void ld_nnp_a(FILE *f, uint16_t val)
{
    op_write3(f, 0xea, val);
}

void xor_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xee, val);
}

void ldh_a_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xf0, val);
}

void pop_af(FILE *f)
{
    op_write1(f, 0xf1);
}

void ld_a_cp(FILE *f)
{
    op_write1(f, 0xf2);
}

void di(FILE *f)
{
    op_write1(f, 0xf3);
}

void push_af(FILE *f)
{
    op_write1(f, 0xf5);
}

void or_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xf6, val);
}

void ldhl_sp_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xf8, val);
}

void ld_sp_hl(FILE *f)
{
    op_write1(f, 0xf9);
}

void ld_a_nnp(FILE *f, uint16_t val)
{
    op_write3(f, 0xfa, val);
}

void ei(FILE *f)
{
    op_write1(f, 0xfb);
}

void cp_n(FILE *f, uint8_t val)
{
    op_write2(f, 0xfe, val);
}

/* CB */

void rlc(FILE *f, register_e reg)
{
    unsigned int opcode = 0x00;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void rrc(FILE *f, register_e reg)
{
    unsigned int opcode = 0x08;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void rl(FILE *f, register_e reg)
{
    unsigned int opcode = 0x10;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void rr(FILE *f, register_e reg)
{
    unsigned int opcode = 0x18;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void sla(FILE *f, register_e reg)
{
    unsigned int opcode = 0x20;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void sra(FILE *f, register_e reg)
{
    unsigned int opcode = 0x28;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void swap(FILE *f, register_e reg)
{
    unsigned int opcode = 0x30;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void srl(FILE *f, register_e reg)
{
    unsigned int opcode = 0x38;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, opcode);
}

void bit(FILE *f, unsigned int b, register_e reg)
{
    if (b > 7) {
        error_bit(b);
    }
    unsigned int opcode = 0x40u + 8u * b;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, (uint8_t)opcode);
}

void res(FILE *f, unsigned int b, register_e reg)
{
    if (b > 7) {
        error_bit(b);
    }
    unsigned int opcode = 0x80u + 8u * b;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, (uint8_t)opcode);
}

void set(FILE *f, unsigned int b, register_e reg)
{
    if (b > 7) {
        error_bit(b);
    }
    unsigned int opcode = 0xc0u + 8u * b;
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
        case REG_HL:
            opcode += 6;
            break;
        case REG_A:
            opcode += 7;
            break;
        default:
            error_register(reg);
    }
    op_write_cb(f, (uint8_t)opcode);
}
