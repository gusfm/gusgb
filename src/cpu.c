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

static void cpu_execute(uint8_t opcode)
{
#ifdef CPU_DEBUG
    cpu_debug(opcode);
#endif
    switch (opcode) {
        case 0x00:
            nop();
            break;
        case 0x01:
            ld_bc_nn(cpu_fetch_word());
            break;
        case 0x02:
            ld_bcp_a();
            break;
        case 0x03:
            inc_bc();
            break;
        case 0x04:
            inc_b();
            break;
        case 0x05:
            dec_b();
            break;
        case 0x06:
            ld_b_n(cpu_fetch_byte());
            break;
        case 0x07:
            rlca();
            break;
        case 0x08:
            ld_nnp_sp(cpu_fetch_word());
            break;
        case 0x09:
            add_hl_bc();
            break;
        case 0x0a:
            ld_a_bcp();
            break;
        case 0x0b:
            dec_bc();
            break;
        case 0x0c:
            inc_c();
            break;
        case 0x0d:
            dec_c();
            break;
        case 0x0e:
            ld_c_n(cpu_fetch_byte());
            break;
        case 0x0f:
            rrca();
            break;
        case 0x10:
            stop();
            break;
        case 0x11:
            ld_de_nn(cpu_fetch_word());
            break;
        case 0x12:
            ld_dep_a();
            break;
        case 0x13:
            inc_de();
            break;
        case 0x14:
            inc_d();
            break;
        case 0x15:
            dec_d();
            break;
        case 0x16:
            ld_d_n(cpu_fetch_byte());
            break;
        case 0x17:
            rla();
            break;
        case 0x18:
            jr_n(cpu_fetch_byte());
            break;
        case 0x19:
            add_hl_de();
            break;
        case 0x1a:
            ld_a_dep();
            break;
        case 0x1b:
            dec_de();
            break;
        case 0x1c:
            inc_e();
            break;
        case 0x1d:
            dec_e();
            break;
        case 0x1e:
            ld_e_n(cpu_fetch_byte());
            break;
        case 0x1f:
            rra();
            break;
        case 0x20:
            jr_nz_n(cpu_fetch_byte());
            break;
        case 0x21:
            ld_hl_nn(cpu_fetch_word());
            break;
        case 0x22:
            ldi_hlp_a();
            break;
        case 0x23:
            inc_hl();
            break;
        case 0x24:
            inc_h();
            break;
        case 0x25:
            dec_h();
            break;
        case 0x26:
            ld_h_n(cpu_fetch_byte());
            break;
        case 0x27:
            daa();
            break;
        case 0x28:
            jr_z_n(cpu_fetch_byte());
            break;
        case 0x29:
            add_hl_hl();
            break;
        case 0x2a:
            ldi_a_hlp();
            break;
        case 0x2b:
            dec_hl();
            break;
        case 0x2c:
            inc_l();
            break;
        case 0x2d:
            dec_l();
            break;
        case 0x2e:
            ld_l_n(cpu_fetch_byte());
            break;
        case 0x2f:
            cpl();
            break;
        case 0x30:
            jr_nc_n(cpu_fetch_byte());
            break;
        case 0x31:
            ld_sp_nn(cpu_fetch_word());
            break;
        case 0x32:
            ldd_hlp_a();
            break;
        case 0x33:
            inc_sp();
            break;
        case 0x34:
            inc_hlp();
            break;
        case 0x35:
            dec_hlp();
            break;
        case 0x36:
            ld_hlp_n(cpu_fetch_byte());
            break;
        case 0x37:
            scf();
            break;
        case 0x38:
            jr_c_n(cpu_fetch_byte());
            break;
        case 0x39:
            add_hl_sp();
            break;
        case 0x3a:
            ldd_a_hlp();
            break;
        case 0x3b:
            dec_sp();
            break;
        case 0x3c:
            inc_a();
            break;
        case 0x3d:
            dec_a();
            break;
        case 0x3e:
            ld_a_n(cpu_fetch_byte());
            break;
        case 0x3f:
            ccf();
            break;
        case 0x40:
            nop();
            break;
        case 0x41:
            ld_b_c();
            break;
        case 0x42:
            ld_b_d();
            break;
        case 0x43:
            ld_b_e();
            break;
        case 0x44:
            ld_b_h();
            break;
        case 0x45:
            ld_b_l();
            break;
        case 0x46:
            ld_b_hlp();
            break;
        case 0x47:
            ld_b_a();
            break;
        case 0x48:
            ld_c_b();
            break;
        case 0x49:
            nop();
            break;
        case 0x4a:
            ld_c_d();
            break;
        case 0x4b:
            ld_c_e();
            break;
        case 0x4c:
            ld_c_h();
            break;
        case 0x4d:
            ld_c_l();
            break;
        case 0x4e:
            ld_c_hlp();
            break;
        case 0x4f:
            ld_c_a();
            break;
        case 0x50:
            ld_d_b();
            break;
        case 0x51:
            ld_d_c();
            break;
        case 0x52:
            nop();
            break;
        case 0x53:
            ld_d_e();
            break;
        case 0x54:
            ld_d_h();
            break;
        case 0x55:
            ld_d_l();
            break;
        case 0x56:
            ld_d_hlp();
            break;
        case 0x57:
            ld_d_a();
            break;
        case 0x58:
            ld_e_b();
            break;
        case 0x59:
            ld_e_c();
            break;
        case 0x5a:
            ld_e_d();
            break;
        case 0x5b:
            nop();
            break;
        case 0x5c:
            ld_e_h();
            break;
        case 0x5d:
            ld_e_l();
            break;
        case 0x5e:
            ld_e_hlp();
            break;
        case 0x5f:
            ld_e_a();
            break;
        case 0x60:
            ld_h_b();
            break;
        case 0x61:
            ld_h_c();
            break;
        case 0x62:
            ld_h_d();
            break;
        case 0x63:
            ld_h_e();
            break;
        case 0x64:
            nop();
            break;
        case 0x65:
            ld_h_l();
            break;
        case 0x66:
            ld_h_hlp();
            break;
        case 0x67:
            ld_h_a();
            break;
        case 0x68:
            ld_l_b();
            break;
        case 0x69:
            ld_l_c();
            break;
        case 0x6a:
            ld_l_d();
            break;
        case 0x6b:
            ld_l_e();
            break;
        case 0x6c:
            ld_l_h();
            break;
        case 0x6d:
            nop();
            break;
        case 0x6e:
            ld_l_hlp();
            break;
        case 0x6f:
            ld_l_a();
            break;
        case 0x70:
            ld_hlp_b();
            break;
        case 0x71:
            ld_hlp_c();
            break;
        case 0x72:
            ld_hlp_d();
            break;
        case 0x73:
            ld_hlp_e();
            break;
        case 0x74:
            ld_hlp_h();
            break;
        case 0x75:
            ld_hlp_l();
            break;
        case 0x76:
            halt();
            break;
        case 0x77:
            ld_hlp_a();
            break;
        case 0x78:
            ld_a_b();
            break;
        case 0x79:
            ld_a_c();
            break;
        case 0x7a:
            ld_a_d();
            break;
        case 0x7b:
            ld_a_e();
            break;
        case 0x7c:
            ld_a_h();
            break;
        case 0x7d:
            ld_a_l();
            break;
        case 0x7e:
            ld_a_hlp();
            break;
        case 0x7f:
            nop();
            break;
        case 0x80:
            add_a_b();
            break;
        case 0x81:
            add_a_c();
            break;
        case 0x82:
            add_a_d();
            break;
        case 0x83:
            add_a_e();
            break;
        case 0x84:
            add_a_h();
            break;
        case 0x85:
            add_a_l();
            break;
        case 0x86:
            add_a_hlp();
            break;
        case 0x87:
            add_a_a();
            break;
        case 0x88:
            adc_b();
            break;
        case 0x89:
            adc_c();
            break;
        case 0x8a:
            adc_d();
            break;
        case 0x8b:
            adc_e();
            break;
        case 0x8c:
            adc_h();
            break;
        case 0x8d:
            adc_l();
            break;
        case 0x8e:
            adc_hlp();
            break;
        case 0x8f:
            adc_a();
            break;
        case 0x90:
            sub_b();
            break;
        case 0x91:
            sub_c();
            break;
        case 0x92:
            sub_d();
            break;
        case 0x93:
            sub_e();
            break;
        case 0x94:
            sub_h();
            break;
        case 0x95:
            sub_l();
            break;
        case 0x96:
            sub_hlp();
            break;
        case 0x97:
            sub_a();
            break;
        case 0x98:
            sbc_b();
            break;
        case 0x99:
            sbc_c();
            break;
        case 0x9a:
            sbc_d();
            break;
        case 0x9b:
            sbc_e();
            break;
        case 0x9c:
            sbc_h();
            break;
        case 0x9d:
            sbc_l();
            break;
        case 0x9e:
            sbc_hlp();
            break;
        case 0x9f:
            sbc_a();
            break;
        case 0xa0:
            and_b();
            break;
        case 0xa1:
            and_c();
            break;
        case 0xa2:
            and_d();
            break;
        case 0xa3:
            and_e();
            break;
        case 0xa4:
            and_h();
            break;
        case 0xa5:
            and_l();
            break;
        case 0xa6:
            and_hlp();
            break;
        case 0xa7:
            and_a();
            break;
        case 0xa8:
            xor_b();
            break;
        case 0xa9:
            xor_c();
            break;
        case 0xaa:
            xor_d();
            break;
        case 0xab:
            xor_e();
            break;
        case 0xac:
            xor_h();
            break;
        case 0xad:
            xor_l();
            break;
        case 0xae:
            xor_hlp();
            break;
        case 0xaf:
            xor_a();
            break;
        case 0xb0:
            or_b();
            break;
        case 0xb1:
            or_c();
            break;
        case 0xb2:
            or_d();
            break;
        case 0xb3:
            or_e();
            break;
        case 0xb4:
            or_h();
            break;
        case 0xb5:
            or_l();
            break;
        case 0xb6:
            or_hlp();
            break;
        case 0xb7:
            or_a();
            break;
        case 0xb8:
            cp_b();
            break;
        case 0xb9:
            cp_c();
            break;
        case 0xba:
            cp_d();
            break;
        case 0xbb:
            cp_e();
            break;
        case 0xbc:
            cp_h();
            break;
        case 0xbd:
            cp_l();
            break;
        case 0xbe:
            cp_hlp();
            break;
        case 0xbf:
            cp_a();
            break;
        case 0xc0:
            ret_nz();
            break;
        case 0xc1:
            pop_bc();
            break;
        case 0xc2:
            jp_nz_nn(cpu_fetch_word());
            break;
        case 0xc3:
            jp_nn(cpu_fetch_word());
            break;
        case 0xc4:
            call_nz_nn(cpu_fetch_word());
            break;
        case 0xc5:
            push_bc();
            break;
        case 0xc6:
            add_a_n(cpu_fetch_byte());
            break;
        case 0xc7:
            rst_00();
            break;
        case 0xc8:
            ret_z();
            break;
        case 0xc9:
            ret();
            break;
        case 0xca:
            jp_z_nn(cpu_fetch_word());
            break;
        case 0xcb:
            cb_n(cpu_fetch_byte());
            break;
        case 0xcc:
            call_z_nn(cpu_fetch_word());
            break;
        case 0xcd:
            call_nn(cpu_fetch_word());
            break;
        case 0xce:
            adc_n(cpu_fetch_byte());
            break;
        case 0xcf:
            rst_08();
            break;
        case 0xd0:
            ret_nc();
            break;
        case 0xd1:
            pop_de();
            break;
        case 0xd2:
            jp_nc_nn(cpu_fetch_word());
            break;
        case 0xd3:
            undefined();
            break;
        case 0xd4:
            call_nc_nn(cpu_fetch_word());
            break;
        case 0xd5:
            push_de();
            break;
        case 0xd6:
            sub_n(cpu_fetch_byte());
            break;
        case 0xd7:
            rst_10();
            break;
        case 0xd8:
            ret_c();
            break;
        case 0xd9:
            reti();
            break;
        case 0xda:
            jp_c_nn(cpu_fetch_word());
            break;
        case 0xdb:
            undefined();
            break;
        case 0xdc:
            call_c_nn(cpu_fetch_word());
            break;
        case 0xdd:
            undefined();
            break;
        case 0xde:
            sbc_n(cpu_fetch_byte());
            break;
        case 0xdf:
            rst_18();
            break;
        case 0xe0:
            ldh_n_a(cpu_fetch_byte());
            break;
        case 0xe1:
            pop_hl();
            break;
        case 0xe2:
            ld_cp_a();
            break;
        case 0xe3:
            undefined();
            break;
        case 0xe4:
            undefined();
            break;
        case 0xe5:
            push_hl();
            break;
        case 0xe6:
            and_n(cpu_fetch_byte());
            break;
        case 0xe7:
            rst_20();
            break;
        case 0xe8:
            add_sp_n(cpu_fetch_byte());
            break;
        case 0xe9:
            jp_hl();
            break;
        case 0xea:
            ld_nnp_a(cpu_fetch_word());
            break;
        case 0xeb:
            undefined();
            break;
        case 0xec:
            undefined();
            break;
        case 0xed:
            undefined();
            break;
        case 0xee:
            xor_n(cpu_fetch_byte());
            break;
        case 0xef:
            rst_28();
            break;
        case 0xf0:
            ldh_a_n(cpu_fetch_byte());
            break;
        case 0xf1:
            pop_af();
            break;
        case 0xf2:
            ld_a_cp();
            break;
        case 0xf3:
            di();
            break;
        case 0xf4:
            undefined();
            break;
        case 0xf5:
            push_af();
            break;
        case 0xf6:
            or_n(cpu_fetch_byte());
            break;
        case 0xf7:
            rst_30();
            break;
        case 0xf8:
            ldhl_sp_n(cpu_fetch_byte());
            break;
        case 0xf9:
            ld_sp_hl();
            break;
        case 0xfa:
            ld_a_nnp(cpu_fetch_word());
            break;
        case 0xfb:
            ei();
            break;
        case 0xfc:
            undefined();
            break;
        case 0xfd:
            undefined();
            break;
        case 0xfe:
            cp_n(cpu_fetch_byte());
            break;
        case 0xff:
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
        clock_step(4);
        CPU.reg.pc--;
    }
}
