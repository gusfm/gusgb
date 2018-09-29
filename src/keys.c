#include "keys.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "interrupt.h"

typedef struct {
    uint8_t rows[2];
    uint8_t column;
} keys_t;

static keys_t KEY;

void keys_reset(void)
{
    KEY.rows[0] = 0x0F;
    KEY.rows[1] = 0x0F;
    KEY.column = 0x10;
}

uint8_t keys_read(void)
{
    uint8_t ret;
    switch (KEY.column) {
        case 0x10:
            ret = KEY.rows[0];
            break;
        case 0x20:
            ret = KEY.rows[1];
            break;
        default:
            ret = 0;
            break;
    }
    return 0xc0 | ret;
}

void keys_write(uint8_t value)
{
    KEY.column = value & 0x30;
}

void key_press(key_e key)
{
    switch (key) {
        case KEY_START:
            KEY.rows[0] &= 0x7;
            break;
        case KEY_SELECT:
            KEY.rows[0] &= 0xb;
            break;
        case KEY_B:
            KEY.rows[0] &= 0xd;
            break;
        case KEY_A:
            KEY.rows[0] &= 0xe;
            break;
        case KEY_DOWN:
            KEY.rows[1] &= 0x7;
            break;
        case KEY_UP:
            KEY.rows[1] &= 0xb;
            break;
        case KEY_LEFT:
            KEY.rows[1] &= 0xd;
            break;
        case KEY_RIGHT:
            KEY.rows[1] &= 0xe;
            break;
        default:
            error("unknown key: %d", key);
    }
    interrupt_raise(INTERRUPTS_JOYPAD);
}

void key_release(key_e key)
{
    switch (key) {
        case KEY_START:
            KEY.rows[0] |= 0x8;
            break;
        case KEY_SELECT:
            KEY.rows[0] |= 0x4;
            break;
        case KEY_B:
            KEY.rows[0] |= 0x2;
            break;
        case KEY_A:
            KEY.rows[0] |= 0x1;
            break;
        case KEY_DOWN:
            KEY.rows[1] |= 0x8;
            break;
        case KEY_UP:
            KEY.rows[1] |= 0x4;
            break;
        case KEY_LEFT:
            KEY.rows[1] |= 0x2;
            break;
        case KEY_RIGHT:
            KEY.rows[1] |= 0x1;
            break;
        default:
            error("unknown key: %d", key);
    }
    interrupt_raise(INTERRUPTS_JOYPAD);
}

bool key_check_pressed(key_e key)
{
    uint8_t val;
    switch (key) {
        case KEY_START:
            val = KEY.rows[0] & 0x8;
            break;
        case KEY_SELECT:
            val = KEY.rows[0] & 0x4;
            break;
        case KEY_B:
            val = KEY.rows[0] & 0x2;
            break;
        case KEY_A:
            val = KEY.rows[0] & 0x1;
            break;
        case KEY_DOWN:
            val = KEY.rows[1] & 0x8;
            break;
        case KEY_UP:
            val = KEY.rows[1] & 0x4;
            break;
        case KEY_LEFT:
            val = KEY.rows[1] & 0x2;
            break;
        case KEY_RIGHT:
            val = KEY.rows[1] & 0x1;
            break;
        default:
            error("unknown key: %d", key);
    }
    return !val;
}

const char *keys_str[] = {"Start", "Select", "B",    "A",
                          "Down",  "Up",     "Left", "Right"};

const char *key_str(key_e key)
{
    return keys_str[key];
}
