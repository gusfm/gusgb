#include "keys.h"
#include <stdio.h>
#include <stdlib.h>
#include "interrupt.h"

typedef struct {
    uint8_t rows[2];
    uint8_t column;
} keys_t;

keys_t KEY;

void keys_init(void)
{
    KEY.rows[0] = 0x0F;
    KEY.rows[1] = 0x0F;
    KEY.column = 0;
}

uint8_t keys_read(void)
{
    switch (KEY.column) {
        case 0x10:
            return KEY.rows[0];
        case 0x20:
            return KEY.rows[1];
        default:
            return 0;
    }
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
            printf("%s:%d: unknown key press: %d\n", __func__, __LINE__, key);
            ;
            break;
    }
    interrupt_set_flag_bit(INTERRUPTS_JOYPAD);
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
            printf("%s:%d: unknown key release: %d\n", __func__, __LINE__, key);
            ;
            break;
    }
    interrupt_set_flag_bit(INTERRUPTS_JOYPAD);
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
            printf("%s:%d: unknown key release: %d\n", __func__, __LINE__, key);
            ;
            exit(EXIT_FAILURE);
    }
    return !val;
}

const char *keys_str[] = {"Start", "Select", "B",    "A",
                          "Down",  "Up",     "Left", "Right"};

const char *key_str(key_e key)
{
    return keys_str[key];
}
