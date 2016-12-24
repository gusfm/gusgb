#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdbool.h>
#include <stdint.h>

#define INTERRUPTS_VBLANK (1 << 0)
#define INTERRUPTS_LCDSTAT (1 << 1)
#define INTERRUPTS_TIMER (1 << 2)
#define INTERRUPTS_SERIAL (1 << 3)
#define INTERRUPTS_JOYPAD (1 << 4)

typedef struct {
    bool master;
    uint8_t enable;
    uint8_t flags;
} interrupt_t;

void interrupt_init(void);

uint8_t interrupt_get_enable(void);
void interrupt_set_enable(uint8_t value);

uint8_t interrupt_get_master(void);
void interrupt_set_master(bool value);

uint8_t interrupt_flag_read(void);
void interrupt_flag_write(uint8_t value);

#endif /* __INTERRUPT_H__ */
