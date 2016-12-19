#ifndef __CPU_OPCODES_H__
#define __CPU_OPCODES_H__

#include "cpu.h"

void nop();
void ld_bc_nn(uint16_t value);
void ld_xbc_a(void);
void inc_bc(void);
void inc_b(void);
void inc_c(void);
void inc_de(void);
void inc_d(void);
void inc_e(void);
void inc_hl(void);
void inc_h(void);
void inc_l(void);
void inc_sp(void);
void inc_xhl(void);
void inc_a(void);

#endif /* __CPU_OPCODES_H__ */
