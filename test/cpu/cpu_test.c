#include <stdint.h>
#include "as/op_enc.h"
#include "asm.h"
#include "clock.h"
#include "cpu.h"
#include "ut.h"

struct ut unit_test;

static uint8_t memory[0x10];
static int mem_pos;

FILE *output;

extern cpu_t CPU;

void cpu_test(void);

static int get_cpu_reg(reg_t reg)
{
    switch (reg) {
        case REG_A:
            return CPU.reg.a;
        case REG_F:
            return CPU.reg.f;
        case REG_B:
            return CPU.reg.b;
        case REG_C:
            return CPU.reg.c;
        case REG_D:
            return CPU.reg.d;
        case REG_E:
            return CPU.reg.e;
        case REG_H:
            return CPU.reg.h;
        case REG_L:
            return CPU.reg.l;
        case REG_AF:
            return CPU.reg.af;
        case REG_BC:
            return CPU.reg.bc;
        case REG_DE:
            return CPU.reg.de;
        case REG_HL:
            return CPU.reg.hl;
        default:
            return -1;
    }
}

static int test_reg(const char *src, int reg, int val, int flags)
{
    mem_pos = 0;
    ASSERT(asm_to_opcode(src, memory, sizeof(memory)) == 0);
    cpu_execute(memory[0]);
    ASSERT_EQ(val, get_cpu_reg(reg));
    ASSERT_EQ(flags, (int)CPU.reg.f);
    return 0;
}

static int adc_test(void)
{
    ASSERT(test_reg("adc a, a", REG_A, 0x00, FLAG_Z) == 0);
    ASSERT(test_reg("adc a, c", REG_A, 0xff, 0) == 0);
    return 0;
}

void cpu_test(void)
{
    cpu_init(NULL);
    CPU.reg.af = 0;
    CPU.reg.bc = 0x00ff;
    CPU.reg.de = 0xff00;
    CPU.reg.hl = 0xffff;
    ut_run(adc_test);
}
