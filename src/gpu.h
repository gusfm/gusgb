#ifndef __GPU_H__
#define __GPU_H__

#include <stdint.h>

uint8_t gpu_get_lcd_control();
uint8_t gpu_get_scroll_y();
uint8_t gpu_get_scroll_x();
uint8_t gpu_get_scanline();

#endif /* __GPU_H__ */
