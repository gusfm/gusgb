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

/* Rotate value right. Old bit 0 to Carry flag. */
uint8_t rrc(uint8_t value)
{
    uint8_t atmp = value;
    value >>= 1;
    if (atmp & 0x01) {
        FLAG_SET(FLAG_C);
        value |= 0x80;
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Rotate value left through Carry flag. */
uint8_t rl(uint8_t value)
{
    uint8_t old_carry = (FLAG_IS_SET(FLAG_C) >> 4);
    if (value & 0x80) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value <<= 1;
    value |= old_carry;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}
