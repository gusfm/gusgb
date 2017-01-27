#ifndef __OPCODES_H__
#define __OPCODES_H__

#include <stdint.h>
#include <stdio.h>

typedef enum {
    REG_A = 0,
    REG_F,
    REG_B,
    REG_C,
    REG_D,
    REG_E,
    REG_H,
    REG_L,
    REG_AF,
    REG_BC,
    REG_DE,
    REG_HL,
    REG_LEN
} register_e;

void data(FILE *f, uint8_t val);
void nop(FILE *f);
void ld_bc_nn(FILE *f, uint16_t val);
void ld_bcp_a(FILE *f);
void inc_bc(FILE *f);
void inc_b(FILE *f);
void dec_b(FILE *f);
void ld_b_n(FILE *f, uint8_t val);
void rlca(FILE *f);
void ld_nnp_sp(FILE *f, uint16_t val);
void add_hl_bc(FILE *f);
void ld_a_bcp(FILE *f);
void dec_bc(FILE *f);
void inc_c(FILE *f);
void dec_c(FILE *f);
void ld_c_n(FILE *f, uint8_t val);
void rrca(FILE *f);
void stop(FILE *f);
void ld_de_nn(FILE *f, uint16_t val);
void ld_dep_a(FILE *f);
void inc_de(FILE *f);
void inc_d(FILE *f);
void dec_d(FILE *f);
void ld_d_n(FILE *f, uint8_t val);
void rla(FILE *f);
void jr_n(FILE *f, uint8_t val);
void add_hl_de(FILE *f);
void ld_a_dep(FILE *f);
void dec_de(FILE *f);
void inc_e(FILE *f);
void dec_e(FILE *f);
void ld_e_n(FILE *f, uint8_t val);
void rra(FILE *f);
void jr_nz_n(FILE *f, uint8_t val);
void ld_hl_nn(FILE *f, uint16_t val);
void ldi_hlp_a(FILE *f);
void inc_hl(FILE *f);
void inc_h(FILE *f);
void dec_h(FILE *f);
void ld_h_n(FILE *f, uint8_t val);
void daa(FILE *f);
void jr_z_n(FILE *f, uint8_t val);
void add_hl_hl(FILE *f);
void ldi_a_hlp(FILE *f);
void dec_hl(FILE *f);
void inc_l(FILE *f);
void dec_l(FILE *f);
void ld_l_n(FILE *f, uint8_t val);
void cpl(FILE *f);
void jr_nc_n(FILE *f, uint8_t val);
void ld_sp_nn(FILE *f, uint16_t val);
void ldd_hlp_a(FILE *f);
void inc_sp(FILE *f);
void inc_hlp(FILE *f);
void dec_hlp(FILE *f);
void ld_hlp_n(FILE *f, uint8_t val);
void scf(FILE *f);
void jr_c_n(FILE *f, uint8_t val);
void add_hl_sp(FILE *f);
void ldd_a_hlp(FILE *f);
void dec_sp(FILE *f);
void inc_a(FILE *f);
void dec_a(FILE *f);
void ld_a_n(FILE *f, uint8_t val);
void ccf(FILE *f);
void ld_b_b(FILE *f);
void ld_b_c(FILE *f);
void ld_b_d(FILE *f);
void ld_b_e(FILE *f);
void ld_b_h(FILE *f);
void ld_b_l(FILE *f);
void ld_b_hlp(FILE *f);
void ld_b_a(FILE *f);
void ld_c_b(FILE *f);
void ld_c_c(FILE *f);
void ld_c_d(FILE *f);
void ld_c_e(FILE *f);
void ld_c_h(FILE *f);
void ld_c_l(FILE *f);
void ld_c_hlp(FILE *f);
void ld_c_a(FILE *f);
void ld_d_b(FILE *f);
void ld_d_c(FILE *f);
void ld_d_d(FILE *f);
void ld_d_e(FILE *f);
void ld_d_h(FILE *f);
void ld_d_l(FILE *f);
void ld_d_hlp(FILE *f);
void ld_d_a(FILE *f);
void ld_e_b(FILE *f);
void ld_e_c(FILE *f);
void ld_e_d(FILE *f);
void ld_e_e(FILE *f);
void ld_e_h(FILE *f);
void ld_e_l(FILE *f);
void ld_e_hlp(FILE *f);
void ld_e_a(FILE *f);
void ld_h_b(FILE *f);
void ld_h_c(FILE *f);
void ld_h_d(FILE *f);
void ld_h_e(FILE *f);
void ld_h_h(FILE *f);
void ld_h_l(FILE *f);
void ld_h_hlp(FILE *f);
void ld_h_a(FILE *f);
void ld_l_b(FILE *f);
void ld_l_c(FILE *f);
void ld_l_d(FILE *f);
void ld_l_e(FILE *f);
void ld_l_h(FILE *f);
void ld_l_l(FILE *f);
void ld_l_hlp(FILE *f);
void ld_l_a(FILE *f);
void ld_hlp_b(FILE *f);
void ld_hlp_c(FILE *f);
void ld_hlp_d(FILE *f);
void ld_hlp_e(FILE *f);
void ld_hlp_h(FILE *f);
void ld_hlp_l(FILE *f);
void halt(FILE *f);
void ld_hlp_a(FILE *f);
void ld_a_b(FILE *f);
void ld_a_c(FILE *f);
void ld_a_d(FILE *f);
void ld_a_e(FILE *f);
void ld_a_h(FILE *f);
void ld_a_l(FILE *f);
void ld_a_hlp(FILE *f);
void ld_a_a(FILE *f);
void add_a_b(FILE *f);
void add_a_c(FILE *f);
void add_a_d(FILE *f);
void add_a_e(FILE *f);
void add_a_h(FILE *f);
void add_a_l(FILE *f);
void add_a_hlp(FILE *f);
void add_a_a(FILE *f);
void adc(FILE *f, register_e reg);
void sub(FILE *f, register_e reg);
void sbc(FILE *f, register_e reg);
void andf(FILE *f, register_e reg);
void xorf(FILE *f, register_e reg);
void orf(FILE *f, register_e reg);
void cp(FILE *f, register_e reg);
void ret_nz(FILE *f);
void pop_bc(FILE *f);
void jp_nz_nn(FILE *f, uint16_t val);
void jp_nn(FILE *f, uint16_t val);
void call_nz_nn(FILE *f, uint16_t val);
void push_bc(FILE *f);
void add_a_n(FILE *f, uint8_t val);
void rst(FILE *f, uint8_t val);
void ret_z(FILE *f);
void ret(FILE *f);
void jp_z_nn(FILE *f, uint16_t val);
void cb_n(FILE *f, uint8_t val);
void call_z_nn(FILE *f, uint16_t val);
void call_nn(FILE *f, uint16_t val);
void adc_n(FILE *f, uint8_t val);
void ret_nc(FILE *f);
void pop_de(FILE *f);
void jp_nc_nn(FILE *f, uint16_t val);
void call_nc_nn(FILE *f, uint16_t val);
void push_de(FILE *f);
void sub_n(FILE *f, uint8_t val);
void ret_c(FILE *f);
void reti(FILE *f);
void jp_c_nn(FILE *f, uint16_t val);
void call_c_nn(FILE *f, uint16_t val);
void sbc_n(FILE *f, uint8_t val);
void ldh_n_a(FILE *f, uint8_t val);
void pop_hl(FILE *f);
void ld_cp_a(FILE *f);
void push_hl(FILE *f);
void and_n(FILE *f, uint8_t val);
void add_sp_n(FILE *f, uint8_t val);
void jp_hl(FILE *f);
void ld_nnp_a(FILE *f, uint16_t val);
void xor_n(FILE *f, uint8_t val);
void ldh_a_n(FILE *f, uint8_t val);
void pop_af(FILE *f);
void ld_a_cp(FILE *f);
void di(FILE *f);
void push_af(FILE *f);
void or_n(FILE *f, uint8_t val);
void ldhl_sp_n(FILE *f, uint8_t val);
void ld_sp_hl(FILE *f);
void ld_a_nnp(FILE *f, uint16_t val);
void ei(FILE *f);
void cp_n(FILE *f, uint8_t val);

/* CB */
void rlc(FILE *f, register_e reg);
void rrc(FILE *f, register_e reg);
void rl(FILE *f, register_e reg);
void rr(FILE *f, register_e reg);
void sla(FILE *f, register_e reg);
void sra(FILE *f, register_e reg);
void swap(FILE *f, register_e reg);
void srl(FILE *f, register_e reg);
void bit(FILE *f, unsigned int b, register_e reg);
void res(FILE *f, unsigned int b, register_e reg);
void set(FILE *f, unsigned int b, register_e reg);

#endif /* __OPCODES_H__ */
