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
void inc_c(void);
void dec_c(void);
void ld_c_n(uint8_t val);
void ld_de_nn(uint16_t value);
void ld_dep_a(void);
void inc_de(void);
void inc_d(void);
void dec_d(void);
void ld_d_n(uint8_t val);
void add_hl_de(void);
void inc_e(void);
void dec_e(void);
void ld_e_n(uint8_t val);
void ld_hl_nn(uint16_t value);
void inc_hl(void);
void inc_h(void);
void dec_h(void);
void ld_h_n(uint8_t val);
void add_hl_hl(void);
void inc_l(void);
void dec_l(void);
void ld_l_n(uint8_t val);
void ld_sp_nn(uint16_t value);
void inc_sp(void);
void inc_hlp(void);
void dec_hlp(void);
void add_hl_sp(void);
void inc_a(void);
void dec_a(void);
void ld_b_a(void);
void ld_c_a(void);
void ld_d_a(void);
void ld_e_a(void);
void ld_h_a(void);
void ld_l_a(void);
void ld_hlp_a(void);
void ld_nnp_a(uint16_t addr);

#endif /* __CPU_OPCODES_H__ */
