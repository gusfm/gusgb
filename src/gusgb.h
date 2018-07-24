#ifndef GUSGB_H
#define GUSGB_H

#include <stdbool.h>

int gusgb_init(int scale, const char *rom_path, bool fullscreen);
void gusgb_finish(void);
void gusgb_main(void);

#endif /* GUSGB_H */
