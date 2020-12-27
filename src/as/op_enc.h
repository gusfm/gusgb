#ifndef __OP_ENC_H__
#define __OP_ENC_H__

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
} reg_t;

void op_enc_init(FILE *output);
unsigned int op_enc_get_pc(void);
void op_enc_jp_addr(unsigned int addr, uint16_t jump_addr);
void op_enc_jr_addr(unsigned int addr, unsigned int jump_addr);

void op_enc_db(uint8_t val);
void op_enc_db_ascii(char *str);
void op_enc_org(long offset);
void op_enc_fill(uint8_t c, size_t n);

void op_enc_nop(void);
void op_enc_ld_bc_nn(uint16_t val);
void op_enc_ld_bcp_a(void);
void op_enc_inc_bc(void);
void op_enc_inc_b(void);
void op_enc_dec_b(void);
void op_enc_ld_b_n(uint8_t val);
void op_enc_rlca(void);
void op_enc_ld_nnp_sp(uint16_t val);
void op_enc_add_hl_bc(void);
void op_enc_ld_a_bcp(void);
void op_enc_dec_bc(void);
void op_enc_inc_c(void);
void op_enc_dec_c(void);
void op_enc_ld_c_n(uint8_t val);
void op_enc_rrca(void);
void op_enc_stop(void);
void op_enc_ld_de_nn(uint16_t val);
void op_enc_ld_dep_a(void);
void op_enc_inc_de(void);
void op_enc_inc_d(void);
void op_enc_dec_d(void);
void op_enc_ld_d_n(uint8_t val);
void op_enc_rla(void);
void op_enc_jr_n(uint8_t val);
void op_enc_add_hl_de(void);
void op_enc_ld_a_dep(void);
void op_enc_dec_de(void);
void op_enc_inc_e(void);
void op_enc_dec_e(void);
void op_enc_ld_e_n(uint8_t val);
void op_enc_rra(void);
void op_enc_jr_nz_n(int8_t val);
void op_enc_ld_hl_nn(uint16_t val);
void op_enc_ldi_hlp_a(void);
void op_enc_inc_hl(void);
void op_enc_inc_h(void);
void op_enc_dec_h(void);
void op_enc_ld_h_n(uint8_t val);
void op_enc_daa(void);
void op_enc_jr_z_n(uint8_t val);
void op_enc_add_hl_hl(void);
void op_enc_ldi_a_hlp(void);
void op_enc_dec_hl(void);
void op_enc_inc_l(void);
void op_enc_dec_l(void);
void op_enc_ld_l_n(uint8_t val);
void op_enc_cpl(void);
void op_enc_jr_nc_n(uint8_t val);
void op_enc_ld_sp_nn(uint16_t val);
void op_enc_ldd_hlp_a(void);
void op_enc_inc_sp(void);
void op_enc_inc_hlp(void);
void op_enc_dec_hlp(void);
void op_enc_ld_hlp_n(uint8_t val);
void op_enc_scf(void);
void op_enc_jr_c_n(uint8_t val);
void op_enc_add_hl_sp(void);
void op_enc_ldd_a_hlp(void);
void op_enc_dec_sp(void);
void op_enc_inc_a(void);
void op_enc_dec_a(void);
void op_enc_ld_a_n(uint8_t val);
void op_enc_ccf(void);
void op_enc_ld_b_reg(reg_t reg);
void op_enc_ld_c_reg(reg_t reg);
void op_enc_ld_d_reg(reg_t reg);
void op_enc_ld_e_reg(reg_t reg);
void op_enc_ld_h_reg(reg_t reg);
void op_enc_ld_l_reg(reg_t reg);
void op_enc_ld_h_b(void);
void op_enc_ld_h_c(void);
void op_enc_ld_h_d(void);
void op_enc_ld_h_e(void);
void op_enc_ld_h_h(void);
void op_enc_ld_h_l(void);
void op_enc_ld_h_hlp(void);
void op_enc_ld_h_a(void);
void op_enc_ld_l_b(void);
void op_enc_ld_l_c(void);
void op_enc_ld_l_d(void);
void op_enc_ld_l_e(void);
void op_enc_ld_l_h(void);
void op_enc_ld_l_l(void);
void op_enc_ld_l_hlp(void);
void op_enc_ld_l_a(void);
void op_enc_ld_hlp_reg(reg_t reg);
void op_enc_halt(void);
void op_enc_ld_a_reg(reg_t reg);
void op_enc_add_a_reg(reg_t reg);
void op_enc_adc(reg_t reg);
void op_enc_sub(reg_t reg);
void op_enc_sbc(reg_t reg);
void op_enc_and(reg_t reg);
void op_enc_xor(reg_t reg);
void op_enc_or(reg_t reg);
void op_enc_cp(reg_t reg);
void op_enc_ret_nz(void);
void op_enc_pop_bc(void);
void op_enc_jp_nz_nn(uint16_t val);
void op_enc_jp_nn(uint16_t val);
void op_enc_call_nz_nn(uint16_t val);
void op_enc_push_bc(void);
void op_enc_add_a_n(uint8_t val);
void op_enc_rst(uint8_t val);
void op_enc_ret_z(void);
void op_enc_ret(void);
void op_enc_jp_z_nn(uint16_t val);
void op_enc_cb_n(uint8_t val);
void op_enc_call_z_nn(uint16_t val);
void op_enc_call_nn(uint16_t val);
void op_enc_adc_n(uint8_t val);
void op_enc_ret_nc(void);
void op_enc_pop_de(void);
void op_enc_jp_nc_nn(uint16_t val);
void op_enc_call_nc_nn(uint16_t val);
void op_enc_push_de(void);
void op_enc_sub_n(uint8_t val);
void op_enc_ret_c(void);
void op_enc_reti(void);
void op_enc_jp_c_nn(uint16_t val);
void op_enc_call_c_nn(uint16_t val);
void op_enc_sbc_n(uint8_t val);
void op_enc_ldh_n_a(uint8_t val);
void op_enc_pop_hl(void);
void op_enc_ld_cp_a(void);
void op_enc_push_hl(void);
void op_enc_and_n(uint8_t val);
void op_enc_add_sp_n(uint8_t val);
void op_enc_jp_hl(void);
void op_enc_ld_nnp_a(uint16_t val);
void op_enc_xor_n(uint8_t val);
void op_enc_ldh_a_n(uint8_t val);
void op_enc_pop_af(void);
void op_enc_ld_a_cp(void);
void op_enc_di(void);
void op_enc_push_af(void);
void op_enc_or_n(uint8_t val);
void op_enc_ldhl_sp_n(uint8_t val);
void op_enc_ld_sp_hl(void);
void op_enc_ld_a_nnp(uint16_t val);
void op_enc_ei(void);
void op_enc_cp_n(uint8_t val);

/* CB */
void op_enc_rlc(reg_t reg);
void op_enc_rrc(reg_t reg);
void op_enc_rl(reg_t reg);
void op_enc_rr(reg_t reg);
void op_enc_sla(reg_t reg);
void op_enc_sra(reg_t reg);
void op_enc_swap(reg_t reg);
void op_enc_srl(reg_t reg);
void op_enc_bit(unsigned int b, reg_t reg);
void op_enc_res(unsigned int b, reg_t reg);
void op_enc_set(unsigned int b, reg_t reg);

#endif /* __OP_ENC_H__ */
