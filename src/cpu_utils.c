#include "cpu_utils.h"
#include "cpu.h"

extern cpu_t g_cpu;

/* Rotate value left. Old bit 7 to Carry flag. */
uint8_t rlc(uint8_t value)
{
    uint8_t carry = (value & 0x80) >> 7;
    if (carry) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value <<= 1;
    value += carry;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}
