#ifndef __KEY_H__
#define __KEY_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    KEY_START = 0,
    KEY_SELECT,
    KEY_B,
    KEY_A,
    KEY_DOWN,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT
} key_e;

void keys_init(void);
uint8_t keys_read(void);
void keys_write(uint8_t value);
void key_press(key_e key);
void key_release(key_e key);
bool key_check_pressed(key_e key);

#endif /* __KEY_H__ */
