#ifndef CLOCK_H
#define CLOCK_H

void clock_reset(void);
extern void clock_step(unsigned int cycles);
extern unsigned int clock_get_step(void);
extern void clock_clear(void);

#endif /* CLOCK_H */
