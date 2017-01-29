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

void ascii(char *str);
void data(uint8_t val);
void jump(long offset);
void memsetf(uint8_t c, size_t n);

void nop(void);
void ld_bc_nn(uint16_t val);
void ld_bcp_a(void);
void inc_bc(void);
void inc_b(void);
void dec_b(void);
void ld_b_n(uint8_t val);
void rlca(void);
void ld_nnp_sp(uint16_t val);
void add_hl_bc(void);
void ld_a_bcp(void);
void dec_bc(void);
void inc_c(void);
void dec_c(void);
void ld_c_n(uint8_t val);
void rrca(void);
void stop(void);
void ld_de_nn(uint16_t val);
void ld_dep_a(void);
void inc_de(void);
void inc_d(void);
void dec_d(void);
void ld_d_n(uint8_t val);
void rla(void);
void jr_n(uint8_t val);
void add_hl_de(void);
void ld_a_dep(void);
void dec_de(void);
void inc_e(void);
void dec_e(void);
void ld_e_n(uint8_t val);
void rra(void);
void jr_nz_n(uint8_t val);
void ld_hl_nn(uint16_t val);
void ldi_hlp_a(void);
void inc_hl(void);
void inc_h(void);
void dec_h(void);
void ld_h_n(uint8_t val);
void daa(void);
void jr_z_n(uint8_t val);
void add_hl_hl(void);
void ldi_a_hlp(void);
void dec_hl(void);
void inc_l(void);
void dec_l(void);
void ld_l_n(uint8_t val);
void cpl(void);
void jr_nc_n(uint8_t val);
void ld_sp_nn(uint16_t val);
void ldd_hlp_a(void);
void inc_sp(void);
void inc_hlp(void);
void dec_hlp(void);
void ld_hlp_n(uint8_t val);
void scf(void);
void jr_c_n(uint8_t val);
void add_hl_sp(void);
void ldd_a_hlp(void);
void dec_sp(void);
void inc_a(void);
void dec_a(void);
void ld_a_n(uint8_t val);
void ccf(void);
void ld_b_b(void);
void ld_b_c(void);
void ld_b_d(void);
void ld_b_e(void);
void ld_b_h(void);
void ld_b_l(void);
void ld_b_hlp(void);
void ld_b_a(void);
void ld_c_b(void);
void ld_c_c(void);
void ld_c_d(void);
void ld_c_e(void);
void ld_c_h(void);
void ld_c_l(void);
void ld_c_hlp(void);
void ld_c_a(void);
void ld_d_b(void);
void ld_d_c(void);
void ld_d_d(void);
void ld_d_e(void);
void ld_d_h(void);
void ld_d_l(void);
void ld_d_hlp(void);
void ld_d_a(void);
void ld_e_b(void);
void ld_e_c(void);
void ld_e_d(void);
void ld_e_e(void);
void ld_e_h(void);
void ld_e_l(void);
void ld_e_hlp(void);
void ld_e_a(void);
void ld_h_b(void);
void ld_h_c(void);
void ld_h_d(void);
void ld_h_e(void);
void ld_h_h(void);
void ld_h_l(void);
void ld_h_hlp(void);
void ld_h_a(void);
void ld_l_b(void);
void ld_l_c(void);
void ld_l_d(void);
void ld_l_e(void);
void ld_l_h(void);
void ld_l_l(void);
void ld_l_hlp(void);
void ld_l_a(void);
void ld_hlp_b(void);
void ld_hlp_c(void);
void ld_hlp_d(void);
void ld_hlp_e(void);
void ld_hlp_h(void);
void ld_hlp_l(void);
void halt(void);
void ld_hlp_a(void);
void ld_a_b(void);
void ld_a_c(void);
void ld_a_d(void);
void ld_a_e(void);
void ld_a_h(void);
void ld_a_l(void);
void ld_a_hlp(void);
void ld_a_a(void);
void add_a_b(void);
void add_a_c(void);
void add_a_d(void);
void add_a_e(void);
void add_a_h(void);
void add_a_l(void);
void add_a_hlp(void);
void add_a_a(void);
void adc(register_e reg);
void sub(register_e reg);
void sbc(register_e reg);
void andf(register_e reg);
void xorf(register_e reg);
void orf(register_e reg);
void cp(register_e reg);
void ret_nz(void);
void pop_bc(void);
void jp_nz_nn(uint16_t val);
void jp_nn(uint16_t val);
void call_nz_nn(uint16_t val);
void push_bc(void);
void add_a_n(uint8_t val);
void rst(uint8_t val);
void ret_z(void);
void ret(void);
void jp_z_nn(uint16_t val);
void cb_n(uint8_t val);
void call_z_nn(uint16_t val);
void call_nn(uint16_t val);
void adc_n(uint8_t val);
void ret_nc(void);
void pop_de(void);
void jp_nc_nn(uint16_t val);
void call_nc_nn(uint16_t val);
void push_de(void);
void sub_n(uint8_t val);
void ret_c(void);
void reti(void);
void jp_c_nn(uint16_t val);
void call_c_nn(uint16_t val);
void sbc_n(uint8_t val);
void ldh_n_a(uint16_t addr, uint8_t val);
void pop_hl(void);
void ld_cp_a(uint16_t addr);
void push_hl(void);
void and_n(uint8_t val);
void add_sp_n(uint8_t val);
void jp_hl(void);
void ld_nnp_a(uint16_t val);
void xor_n(uint8_t val);
void ldh_a_n(uint16_t addr, uint8_t val);
void pop_af(void);
void ld_a_cp(uint16_t addr);
void di(void);
void push_af(void);
void or_n(uint8_t val);
void ldhl_sp_n(uint8_t val);
void ld_sp_hl(void);
void ld_a_nnp(uint16_t val);
void ei(void);
void cp_n(uint8_t val);

/* CB */
void rlc(register_e reg);
void rrc(register_e reg);
void rl(register_e reg);
void rr(register_e reg);
void sla(register_e reg);
void sra(register_e reg);
void swap(register_e reg);
void srl(register_e reg);
void bit(unsigned int b, register_e reg);
void res(unsigned int b, register_e reg);
void set(unsigned int b, register_e reg);

#endif /* __OPCODES_H__ */
