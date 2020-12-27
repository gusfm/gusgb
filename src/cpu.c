#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apu/apu.h"
#include "cartridge/cart.h"
#include "clock.h"
#include "cpu_ext_ops.h"
#include "cpu_opcodes.h"
#include "debug.h"
#include "gpu.h"
#include "interrupt.h"
#include "mmu.h"
#ifdef CPU_DEBUG
#include "cpu_debug.h"
#endif

cpu_t CPU;

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
        CPU.reg.de = 0x0008;
        CPU.reg.hl = 0x007c;
    } else {
        CPU.reg.af = 0x01b0;
        CPU.reg.bc = 0x0013;
        CPU.reg.de = 0x00d8;
        CPU.reg.hl = 0x014d;
    }
    clock_reset();
    mmu_reset();
}

static inline uint8_t cpu_fetch_byte(void)
{
    return mmu_read_byte(CPU.reg.pc++);
}

static inline uint16_t cpu_fetch_word(void)
{
    uint16_t word = mmu_read_word(CPU.reg.pc);
    CPU.reg.pc += 2;
    return word;
}

void cpu_execute(uint8_t opcode)
{
#ifdef CPU_DEBUG
    cpu_debug(opcode);
#endif
    switch (opcode) {
        case 0x00: /* NOP */
            nop();
            break;
        case 0x01: /* LD BC,NN */
            ld_bc_nn(cpu_fetch_word());
            break;
        case 0x02: /* LD (BC),A */
            ld_bcp_a();
            break;
        case 0x03: /* INC BC */
            inc_bc();
            break;
        case 0x04: /* INC B */
            inc_b();
            break;
        case 0x05: /* DEC B */
            dec_b();
            break;
        case 0x06: /* LD B,N */
            ld_b_n(cpu_fetch_byte());
            break;
        case 0x07: /* RLCA */
            rlca();
            break;
        case 0x08: /* LD (NN),SP */
            ld_nnp_sp(cpu_fetch_word());
            break;
        case 0x09: /* ADD HL,BC */
            add_hl_bc();
            break;
        case 0x0a: /* LD A,(BC) */
            ld_a_bcp();
            break;
        case 0x0b: /* DEC BC */
            dec_bc();
            break;
        case 0x0c: /* INC C */
            inc_c();
            break;
        case 0x0d: /* DEC C */
            dec_c();
            break;
        case 0x0e: /* LD C,N */
            ld_c_n(cpu_fetch_byte());
            break;
        case 0x0f: /* RRCA */
            rrca();
            break;
        case 0x10: /* STOP */
            stop();
            break;
        case 0x11: /* LD DE,NN */
            ld_de_nn(cpu_fetch_word());
            break;
        case 0x12: /* LD (DE),A */
            ld_dep_a();
            break;
        case 0x13: /* INC DE */
            inc_de();
            break;
        case 0x14: /* INC D */
            inc_d();
            break;
        case 0x15: /* DEC D */
            dec_d();
            break;
        case 0x16: /* LD D,N */
            ld_d_n(cpu_fetch_byte());
            break;
        case 0x17: /* RLA */
            rla();
            break;
        case 0x18: /* JR N */
            jr_n(cpu_fetch_byte());
            break;
        case 0x19: /* ADD HL,DE */
            add_hl_de();
            break;
        case 0x1a: /* LD A,(DE) */
            ld_a_dep();
            break;
        case 0x1b: /* DEC DE */
            dec_de();
            break;
        case 0x1c: /* INC E */
            inc_e();
            break;
        case 0x1d: /* DEC E */
            dec_e();
            break;
        case 0x1e: /* LD E,N */
            ld_e_n(cpu_fetch_byte());
            break;
        case 0x1f: /* RRA */
            rra();
            break;
        case 0x20: /* JR NZ,N */
            jr_nz_n(cpu_fetch_byte());
            break;
        case 0x21: /* LD HL,NN */
            ld_hl_nn(cpu_fetch_word());
            break;
        case 0x22: /* LDI (HL),A */
            ldi_hlp_a();
            break;
        case 0x23: /* INC HL */
            inc_hl();
            break;
        case 0x24: /* INC H */
            inc_h();
            break;
        case 0x25: /* DEC H */
            dec_h();
            break;
        case 0x26: /* LD H,N */
            ld_h_n(cpu_fetch_byte());
            break;
        case 0x27: /* DAA */
            daa();
            break;
        case 0x28: /* JR Z,N */
            jr_z_n(cpu_fetch_byte());
            break;
        case 0x29: /* ADD HL,HL */
            add_hl_hl();
            break;
        case 0x2a: /* LDI A,(HL) */
            ldi_a_hlp();
            break;
        case 0x2b: /* DEC HL */
            dec_hl();
            break;
        case 0x2c: /* INC L */
            inc_l();
            break;
        case 0x2d: /* DEC L */
            dec_l();
            break;
        case 0x2e: /* LD L,N */
            ld_l_n(cpu_fetch_byte());
            break;
        case 0x2f: /* CPL */
            cpl();
            break;
        case 0x30: /* JR NC,N */
            jr_nc_n(cpu_fetch_byte());
            break;
        case 0x31: /* LD SP,NN */
            ld_sp_nn(cpu_fetch_word());
            break;
        case 0x32: /* LDD (HL),A */
            ldd_hlp_a();
            break;
        case 0x33: /* INC SP */
            inc_sp();
            break;
        case 0x34: /* INC (HL) */
            inc_hlp();
            break;
        case 0x35: /* DEC (HL) */
            dec_hlp();
            break;
        case 0x36: /* LD (HL),N */
            ld_hlp_n(cpu_fetch_byte());
            break;
        case 0x37: /* SCF */
            scf();
            break;
        case 0x38: /* JR C,N */
            jr_c_n(cpu_fetch_byte());
            break;
        case 0x39: /* ADD HL,SP */
            add_hl_sp();
            break;
        case 0x3a: /* LDD A,(HL) */
            ldd_a_hlp();
            break;
        case 0x3b: /* DEC SP */
            dec_sp();
            break;
        case 0x3c: /* INC A */
            inc_a();
            break;
        case 0x3d: /* DEC A */
            dec_a();
            break;
        case 0x3e: /* LD A,N */
            ld_a_n(cpu_fetch_byte());
            break;
        case 0x3f: /* CCF */
            ccf();
            break;
        case 0x40: /* LD B,B */
            nop();
            break;
        case 0x41: /* LD B,C */
            ld_b_c();
            break;
        case 0x42: /* LD B,D */
            ld_b_d();
            break;
        case 0x43: /* LD B,E */
            ld_b_e();
            break;
        case 0x44: /* LD B,H */
            ld_b_h();
            break;
        case 0x45: /* LD B,L */
            ld_b_l();
            break;
        case 0x46: /* LD B,(HL) */
            ld_b_hlp();
            break;
        case 0x47: /* LD B,A */
            ld_b_a();
            break;
        case 0x48: /* LD C,B */
            ld_c_b();
            break;
        case 0x49: /* LD C,C */
            nop();
            break;
        case 0x4a: /* LD C,D */
            ld_c_d();
            break;
        case 0x4b: /* LD C,E */
            ld_c_e();
            break;
        case 0x4c: /* LD C,H */
            ld_c_h();
            break;
        case 0x4d: /* LD C,L */
            ld_c_l();
            break;
        case 0x4e: /* LD C,(HL) */
            ld_c_hlp();
            break;
        case 0x4f: /* LD C,A */
            ld_c_a();
            break;
        case 0x50: /* LD D,B */
            ld_d_b();
            break;
        case 0x51: /* LD D,C */
            ld_d_c();
            break;
        case 0x52: /* LD D,D */
            nop();
            break;
        case 0x53: /* LD D,E */
            ld_d_e();
            break;
        case 0x54: /* LD D,H */
            ld_d_h();
            break;
        case 0x55: /* LD D,L */
            ld_d_l();
            break;
        case 0x56: /* LD D,(HL) */
            ld_d_hlp();
            break;
        case 0x57: /* LD D,A */
            ld_d_a();
            break;
        case 0x58: /* LD E,B */
            ld_e_b();
            break;
        case 0x59: /* LD E,C */
            ld_e_c();
            break;
        case 0x5a: /* LD E,D */
            ld_e_d();
            break;
        case 0x5b: /* LD E,E */
            nop();
            break;
        case 0x5c: /* LD E,H */
            ld_e_h();
            break;
        case 0x5d: /* LD E,L */
            ld_e_l();
            break;
        case 0x5e: /* LD E,(HL) */
            ld_e_hlp();
            break;
        case 0x5f: /* LD E,A */
            ld_e_a();
            break;
        case 0x60: /* LD H,B */
            ld_h_b();
            break;
        case 0x61: /* LD H,C */
            ld_h_c();
            break;
        case 0x62: /* LD H,D */
            ld_h_d();
            break;
        case 0x63: /* LD H,E */
            ld_h_e();
            break;
        case 0x64: /* LD H,H */
            nop();
            break;
        case 0x65: /* LD H,L */
            ld_h_l();
            break;
        case 0x66: /* LD H,(HL) */
            ld_h_hlp();
            break;
        case 0x67: /* LD H,A */
            ld_h_a();
            break;
        case 0x68: /* LD L,B */
            ld_l_b();
            break;
        case 0x69: /* LD L,C */
            ld_l_c();
            break;
        case 0x6a: /* LD L,D */
            ld_l_d();
            break;
        case 0x6b: /* LD L,E */
            ld_l_e();
            break;
        case 0x6c: /* LD L,H */
            ld_l_h();
            break;
        case 0x6d: /* LD L,L */
            nop();
            break;
        case 0x6e: /* LD L,(HL) */
            ld_l_hlp();
            break;
        case 0x6f: /* LD L,A */
            ld_l_a();
            break;
        case 0x70: /* LD (HL),B */
            ld_hlp_b();
            break;
        case 0x71: /* LD (HL),C */
            ld_hlp_c();
            break;
        case 0x72: /* LD (HL),D */
            ld_hlp_d();
            break;
        case 0x73: /* LD (HL),E */
            ld_hlp_e();
            break;
        case 0x74: /* LD (HL),H */
            ld_hlp_h();
            break;
        case 0x75: /* LD (HL),L */
            ld_hlp_l();
            break;
        case 0x76: /* HALT */
            halt();
            break;
        case 0x77: /* LD (HL),A */
            ld_hlp_a();
            break;
        case 0x78: /* LD A,B */
            ld_a_b();
            break;
        case 0x79: /* LD A,C */
            ld_a_c();
            break;
        case 0x7a: /* LD A,D */
            ld_a_d();
            break;
        case 0x7b: /* LD A,E */
            ld_a_e();
            break;
        case 0x7c: /* LD A,H */
            ld_a_h();
            break;
        case 0x7d: /* LD A,L */
            ld_a_l();
            break;
        case 0x7e: /* LD A,(HL) */
            ld_a_hlp();
            break;
        case 0x7f: /* LD A,A */
            nop();
            break;
        case 0x80: /* ADD A,B */
            add_a_b();
            break;
        case 0x81: /* ADD A,C */
            add_a_c();
            break;
        case 0x82: /* ADD A,D */
            add_a_d();
            break;
        case 0x83: /* ADD A,E */
            add_a_e();
            break;
        case 0x84: /* ADD A,H */
            add_a_h();
            break;
        case 0x85: /* ADD A,L */
            add_a_l();
            break;
        case 0x86: /* ADD A,(HL) */
            add_a_hlp();
            break;
        case 0x87: /* ADD A,A */
            add_a_a();
            break;
        case 0x88: /* ADC B */
            adc_b();
            break;
        case 0x89: /* ADC C */
            adc_c();
            break;
        case 0x8a: /* ADC D */
            adc_d();
            break;
        case 0x8b: /* ADC E */
            adc_e();
            break;
        case 0x8c: /* ADC H */
            adc_h();
            break;
        case 0x8d: /* ADC L */
            adc_l();
            break;
        case 0x8e: /* ADC (HL) */
            adc_hlp();
            break;
        case 0x8f: /* ADC A */
            adc_a();
            break;
        case 0x90: /* SUB B */
            sub_b();
            break;
        case 0x91: /* SUB C */
            sub_c();
            break;
        case 0x92: /* SUB D */
            sub_d();
            break;
        case 0x93: /* SUB E */
            sub_e();
            break;
        case 0x94: /* SUB H */
            sub_h();
            break;
        case 0x95: /* SUB L */
            sub_l();
            break;
        case 0x96: /* SUB (HL) */
            sub_hlp();
            break;
        case 0x97: /* SUB A */
            sub_a();
            break;
        case 0x98: /* SBC B */
            sbc_b();
            break;
        case 0x99: /* SBC C */
            sbc_c();
            break;
        case 0x9a: /* SBC D */
            sbc_d();
            break;
        case 0x9b: /* SBC E */
            sbc_e();
            break;
        case 0x9c: /* SBC H */
            sbc_h();
            break;
        case 0x9d: /* SBC L */
            sbc_l();
            break;
        case 0x9e: /* SBC (HL) */
            sbc_hlp();
            break;
        case 0x9f: /* SBC A */
            sbc_a();
            break;
        case 0xa0: /* AND B */
            and_b();
            break;
        case 0xa1: /* AND C */
            and_c();
            break;
        case 0xa2: /* AND D */
            and_d();
            break;
        case 0xa3: /* AND E */
            and_e();
            break;
        case 0xa4: /* AND H */
            and_h();
            break;
        case 0xa5: /* AND L */
            and_l();
            break;
        case 0xa6: /* AND (HL) */
            and_hlp();
            break;
        case 0xa7: /* AND A */
            and_a();
            break;
        case 0xa8: /* XOR B */
            xor_b();
            break;
        case 0xa9: /* XOR C */
            xor_c();
            break;
        case 0xaa: /* XOR D */
            xor_d();
            break;
        case 0xab: /* XOR E */
            xor_e();
            break;
        case 0xac: /* XOR H */
            xor_h();
            break;
        case 0xad: /* XOR L */
            xor_l();
            break;
        case 0xae: /* XOR (HL) */
            xor_hlp();
            break;
        case 0xaf: /* XOR A */
            xor_a();
            break;
        case 0xb0: /* OR B */
            or_b();
            break;
        case 0xb1: /* OR C */
            or_c();
            break;
        case 0xb2: /* OR D */
            or_d();
            break;
        case 0xb3: /* OR E */
            or_e();
            break;
        case 0xb4: /* OR H */
            or_h();
            break;
        case 0xb5: /* OR L */
            or_l();
            break;
        case 0xb6: /* OR (HL) */
            or_hlp();
            break;
        case 0xb7: /* OR A */
            or_a();
            break;
        case 0xb8: /* CP B */
            cp_b();
            break;
        case 0xb9: /* CP C */
            cp_c();
            break;
        case 0xba: /* CP D */
            cp_d();
            break;
        case 0xbb: /* CP E */
            cp_e();
            break;
        case 0xbc: /* CP H */
            cp_h();
            break;
        case 0xbd: /* CP L */
            cp_l();
            break;
        case 0xbe: /* CP (HL) */
            cp_hlp();
            break;
        case 0xbf: /* CP A */
            cp_a();
            break;
        case 0xc0: /* RET NZ */
            ret_nz();
            break;
        case 0xc1: /* POP BC */
            pop_bc();
            break;
        case 0xc2: /* JP NZ,NN */
            jp_nz_nn(cpu_fetch_word());
            break;
        case 0xc3: /* JP NN */
            jp_nn(cpu_fetch_word());
            break;
        case 0xc4: /* CALL NZ,NN */
            call_nz_nn(cpu_fetch_word());
            break;
        case 0xc5: /* PUSH BC */
            push_bc();
            break;
        case 0xc6: /* ADD A,N */
            add_a_n(cpu_fetch_byte());
            break;
        case 0xc7: /* RST 00 */
            rst_00();
            break;
        case 0xc8: /* RET Z */
            ret_z();
            break;
        case 0xc9: /* RET */
            ret();
            break;
        case 0xca: /* JP Z,NN */
            jp_z_nn(cpu_fetch_word());
            break;
        case 0xcb: /* CB N */
            cb_n(cpu_fetch_byte());
            break;
        case 0xcc: /* CALL Z,NN */
            call_z_nn(cpu_fetch_word());
            break;
        case 0xcd: /* CALL NN */
            call_nn(cpu_fetch_word());
            break;
        case 0xce: /* ADC N */
            adc_n(cpu_fetch_byte());
            break;
        case 0xcf: /* RST 08 */
            rst_08();
            break;
        case 0xd0: /* RET NC */
            ret_nc();
            break;
        case 0xd1: /* POP DE */
            pop_de();
            break;
        case 0xd2: /* JP NC,NN */
            jp_nc_nn(cpu_fetch_word());
            break;
        case 0xd3: /* UNDEFINED */
            undefined();
            break;
        case 0xd4: /* CALL NC,NN */
            call_nc_nn(cpu_fetch_word());
            break;
        case 0xd5: /* PUSH DE */
            push_de();
            break;
        case 0xd6: /* SUB N */
            sub_n(cpu_fetch_byte());
            break;
        case 0xd7: /* RST 10 */
            rst_10();
            break;
        case 0xd8: /* RET C */
            ret_c();
            break;
        case 0xd9: /* RETI */
            reti();
            break;
        case 0xda: /* JP C,NN */
            jp_c_nn(cpu_fetch_word());
            break;
        case 0xdb: /* UNDEFINED */
            undefined();
            break;
        case 0xdc: /* CALL C,NN */
            call_c_nn(cpu_fetch_word());
            break;
        case 0xdd: /* UNDEFINED */
            undefined();
            break;
        case 0xde: /* SBC N */
            sbc_n(cpu_fetch_byte());
            break;
        case 0xdf: /* RST 18 */
            rst_18();
            break;
        case 0xe0: /* LDH N,A */
            ldh_n_a(cpu_fetch_byte());
            break;
        case 0xe1: /* POP HL */
            pop_hl();
            break;
        case 0xe2: /* LD CP,A */
            ld_cp_a();
            break;
        case 0xe3: /* UNDEFINED */
            undefined();
            break;
        case 0xe4: /* UNDEFINED */
            undefined();
            break;
        case 0xe5: /* PUSH HL */
            push_hl();
            break;
        case 0xe6: /* AND N */
            and_n(cpu_fetch_byte());
            break;
        case 0xe7: /* RST 20 */
            rst_20();
            break;
        case 0xe8: /* ADD SP,N */
            add_sp_n(cpu_fetch_byte());
            break;
        case 0xe9: /* JP HL */
            jp_hl();
            break;
        case 0xea: /* LD (NN),A */
            ld_nnp_a(cpu_fetch_word());
            break;
        case 0xeb: /* UNDEFINED */
            undefined();
            break;
        case 0xec: /* UNDEFINED */
            undefined();
            break;
        case 0xed: /* UNDEFINED */
            undefined();
            break;
        case 0xee: /* XOR N */
            xor_n(cpu_fetch_byte());
            break;
        case 0xef: /* RST 28 */
            rst_28();
            break;
        case 0xf0: /* LDH A,N */
            ldh_a_n(cpu_fetch_byte());
            break;
        case 0xf1: /* POP AF */
            pop_af();
            break;
        case 0xf2: /* LD A,CP */
            ld_a_cp();
            break;
        case 0xf3: /* DI */
            di();
            break;
        case 0xf4: /* UNDEFINED */
            undefined();
            break;
        case 0xf5: /* PUSH AF */
            push_af();
            break;
        case 0xf6: /* OR N */
            or_n(cpu_fetch_byte());
            break;
        case 0xf7: /* RST 30 */
            rst_30();
            break;
        case 0xf8: /* LDHL SP,N */
            ldhl_sp_n(cpu_fetch_byte());
            break;
        case 0xf9: /* LD SP,HL */
            ld_sp_hl();
            break;
        case 0xfa: /* LD A,(NN) */
            ld_a_nnp(cpu_fetch_word());
            break;
        case 0xfb: /* EI */
            ei();
            break;
        case 0xfc: /* UNDEFINED */
            undefined();
            break;
        case 0xfd: /* UNDEFINED */
            undefined();
            break;
        case 0xfe: /* CP N */
            cp_n(cpu_fetch_byte());
            break;
        case 0xff: /* RST 38 */
            rst_38();
            break;
    }
}

void cpu_emulate_cycle(void)
{
    clock_clear();
    cpu_execute(cpu_fetch_byte());
    interrupt_step();
    apu_tick(clock_get_step());
    gpu_tick(clock_get_step());
}

void cpu_halted(void)
{
    if (CPU.halt_bug) {
        uint8_t opcode = cpu_fetch_byte();
        CPU.reg.pc--;
        cpu_execute(opcode);
        CPU.halt = false;
        CPU.halt_bug = false;
    } else {
        CPU.reg.pc--;
    }
}
