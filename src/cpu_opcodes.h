#ifndef __CPU_OPCODES_H__
#define __CPU_OPCODES_H__

#include "cpu.h"

void nop();
void ld_bc_nn(uint16_t value);
void ld_bcp_a(void);
void inc_bc(void);
void inc_b(void);
void dec_b(void);
void ld_b_n(uint8_t val);
void rlca(void);
void ld_nnp_sp(uint16_t addr);
void add_hl_bc(void);
void ld_a_bcp(void);
void dec_bc(void);
void inc_c(void);
void dec_c(void);
void ld_c_n(uint8_t val);
void rrca(void);
void stop(void);
void ld_de_nn(uint16_t value);
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
void ld_hl_nn(uint16_t value);
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
void ld_sp_nn(uint16_t value);
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
void ld_b_c(uint8_t val);
void ld_b_d(uint8_t val);
void ld_b_e(uint8_t val);
void ld_b_h(uint8_t val);
void ld_b_l(uint8_t val);
void ld_b_hlp(void);
void ld_b_a(void);
void ld_c_b(void);
void ld_c_d(void);
void ld_c_e(void);
void ld_c_h(void);
void ld_c_l(void);
void ld_c_hlp(void);
void ld_c_a(void);
void ld_d_b(void);
void ld_d_c(void);
void ld_d_e(void);
void ld_d_h(void);
void ld_d_l(void);
void ld_d_hlp(void);
void ld_d_a(void);
void ld_e_b(void);
void ld_e_c(void);
void ld_e_d(void);
void ld_e_h(void);
void ld_e_l(void);
void ld_e_hlp(void);
void ld_e_a(void);
void ld_h_b(void);
void ld_h_c(void);
void ld_h_d(void);
void ld_h_e(void);
void ld_h_l(void);
void ld_h_hlp(void);
void ld_h_a(void);
void ld_l_b(void);
void ld_l_c(void);
void ld_l_d(void);
void ld_l_e(void);
void ld_l_h(void);
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
void add_a_b(void);
void add_a_c(void);
void add_a_d(void);
void add_a_e(void);
void add_a_h(void);
void add_a_l(void);
void add_a_hlp(void);
void add_a_a(void);
void adc_b(void);
void adc_c(void);
void adc_d(void);
void adc_e(void);
void adc_h(void);
void adc_l(void);
void adc_hlp(void);
void adc_a(void);
void sub_b(void);
void sub_c(void);
void sub_d(void);
void sub_e(void);
void sub_h(void);
void sub_l(void);
void sub_hlp(void);
void sub_a(void);
void sbc_b(void);
void sbc_c(void);
void sbc_d(void);
void sbc_e(void);
void sbc_h(void);
void sbc_l(void);
void sbc_hlp(void);
void sbc_a(void);
void and_b(void);
void and_c(void);
void and_d(void);
void and_e(void);
void and_h(void);
void and_l(void);
void and_hlp(void);
void and_a(void);
void xor_b(void);
void xor_c(void);
void xor_d(void);
void xor_e(void);
void xor_h(void);
void xor_l(void);
void xor_hlp(void);
void xor_a(void);
void or_b(void);
void or_c(void);
void or_d(void);
void or_e(void);
void or_h(void);
void or_l(void);
void or_hlp(void);
void or_a(void);
void cp_b(void);
void cp_c(void);
void cp_d(void);
void cp_e(void);
void cp_h(void);
void cp_l(void);
void cp_hlp(void);
void cp_a(void);
void ret_nz(void);
void add_a_n(uint8_t val);
void ret_z(void);
void ret(void);
void adc_n(uint8_t n);
void ret_nc(void);
void sub_n(uint8_t val);
void ret_c(void);
void sbc_n(uint8_t val);
void and_n(uint8_t val);
void ld_nnp_a(uint16_t addr);
void xor_n(uint8_t val);
void or_n(uint8_t val);
void ld_a_nnp(uint16_t addr);
void cp_n(uint8_t val);

#endif /* __CPU_OPCODES_H__ */
