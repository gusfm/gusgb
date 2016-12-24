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

/* Rotate value right through Carry flag. */
uint8_t rr(uint8_t value)
{
    uint8_t old_carry = (FLAG_IS_SET(FLAG_C) << 3);
    if (value & 0x01) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value >>= 1;
    value |= old_carry;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Shift value left into Carry. */
uint8_t sla(uint8_t value)
{
    if (value & 0x80) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value <<= 1;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Shift value right into Carry flag. */
uint8_t sra(uint8_t value)
{
    if (value & 0x01) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value = (value & 0x80) | (value >> 1);
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

uint8_t swap(uint8_t value)
{
    value = ((value & 0x0f) << 4) | ((value & 0xf0) >> 4);
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
    return value;
}

/* Shift value right into Carry flag. MSB set to 0. */
uint8_t srl(uint8_t value)
{
    if (value & 0x01) {
        FLAG_SET(FLAG_C);
    } else {
        FLAG_CLEAR(FLAG_C);
    }
    value >>= 1;
    if (value == 0) {
        FLAG_SET(FLAG_Z);
    } else {
        FLAG_CLEAR(FLAG_Z);
    }
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}
