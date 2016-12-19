#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

#define INTERRUPTS_VBLANK	(1 << 0)
#define INTERRUPTS_LCDSTAT	(1 << 1)
#define INTERRUPTS_TIMER	(1 << 2)
#define INTERRUPTS_SERIAL	(1 << 3)
#define INTERRUPTS_JOYPAD	(1 << 4)

typedef struct {
	uint8_t master;
	uint8_t enable;
	uint8_t flags;
} interrupt_t;

void interrupt_init(void);

uint8_t interrupt_enable_read(void);

void interrupt_enable_write(uint8_t value);

uint8_t interrupt_flag_read(void);

void interrupt_flag_write(uint8_t value);

#endif /* __INTERRUPT_H__ */
