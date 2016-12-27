#ifndef __GPU_H__
#define __GPU_H__

#include <stdint.h>

uint8_t gpu_get_lcd_control(void);
uint8_t gpu_get_scroll_y(void);
uint8_t gpu_get_scroll_x(void);
uint8_t gpu_get_scanline(void);

#endif /* __GPU_H__ */
