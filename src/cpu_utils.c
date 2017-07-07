#include "cpu_utils.h"
#include "cpu.h"

extern cpu_t g_cpu;

/* Rotate value left. Old bit 7 to Carry flag. */
uint8_t rlc(uint8_t value)
{
    uint8_t carry = (value & 0x80) >> 7;
    FLAG_SET_CARRY(carry);
    value = (value << 1) | carry;
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Rotate value right. Old bit 0 to Carry flag. */
uint8_t rrc(uint8_t value)
{
    FLAG_SET_CARRY(value);
    value = (value << 7) | (value >> 1);
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Rotate value left through Carry flag. */
uint8_t rl(uint8_t value)
{
    uint8_t old_carry = (uint8_t)(FLAG_IS_SET(FLAG_C) >> 4);
    FLAG_SET_CARRY((value & 0x80) >> 7);
    value = (value << 1) | old_carry;
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Rotate value right through Carry flag. */
uint8_t rr(uint8_t value)
{
    uint8_t old_carry = (uint8_t)(FLAG_IS_SET(FLAG_C) << 3);
    FLAG_SET_CARRY(value);
    value = old_carry | value >> 1;
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Shift value left into Carry. */
uint8_t sla(uint8_t value)
{
    FLAG_SET_CARRY(value >> 7);
    value = (uint8_t)(value << 1);
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

/* Shift value right into Carry flag. */
uint8_t sra(uint8_t value)
{
    FLAG_SET_CARRY(value);
    value = (uint8_t)((value & 0x80) | (value >> 1));
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

uint8_t swap(uint8_t value)
{
    value = (uint8_t)(((value & 0x0f) << 4) | ((value & 0xf0) >> 4));
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H | FLAG_C);
    return value;
}

/* Shift value right into Carry flag. MSB set to 0. */
uint8_t srl(uint8_t value)
{
    FLAG_SET_CARRY(value);
    value >>= 1;
    FLAG_SET_ZERO(!value);
    FLAG_CLEAR(FLAG_N | FLAG_H);
    return value;
}

void bit(uint8_t bit, uint8_t value)
{
    FLAG_SET_ZERO(!(value & bit));
    FLAG_CLEAR(FLAG_N);
    FLAG_SET(FLAG_H);
}

uint8_t res(uint8_t bit, uint8_t value)
{
    return (uint8_t)(value & ~bit);
}

uint8_t set(uint8_t bit, uint8_t value)
{
    return (uint8_t)(value | bit);
}
