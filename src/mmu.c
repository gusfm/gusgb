#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpu.h"
#include "interrupt.h"
#include "rom.h"

mmu_t g_mmu;

int mmu_init(const char *rom_path)
{
    int ret = rom_load(rom_path, g_mmu.cart, sizeof(g_mmu.cart));
    if (ret < 0) {
        return -1;
    }
    memset(g_mmu.sram, 0, sizeof(g_mmu.sram));
    // memcpy(g_mmu.io, io_reset, sizeof(g_mmu.io));
    memset(g_mmu.vram, 0, sizeof(g_mmu.vram));
    memset(g_mmu.oam, 0, sizeof(g_mmu.oam));
    memset(g_mmu.wram, 0, sizeof(g_mmu.wram));
    memset(g_mmu.hram, 0, sizeof(g_mmu.hram));
    g_mmu.read_internal_rom = true;
    g_mmu.cart_type = g_mmu.cart[ROM_OFFSET_TYPE];
    interrupt_init();
    gpu_init(g_mmu.vram, g_mmu.oam);
    return 0;
}

static uint8_t mmu_read_byte_ffxx(uint16_t addr)
{
    uint8_t ret;
    /* I/O Registers. */
    switch (addr) {
        case 0xff00:
            /* P1 (R/W): Register for reading joy pad info. */
            if (!(g_mmu.io[0x00] & 0x20)) {
                return (unsigned char)(0xc0 |0 /* keys.keys1 */ | 0x10);
            }

            else if (!(g_mmu.io[0x00] & 0x10)) {
                return (unsigned char)(0xc0 | 0 /* keys.keys2 */ | 0x20);
            }

            else if (!(g_mmu.io[0x00] & 0x30))
                return 0xff;
            else
                return 0;
        case 0xff01:
        /* SB (R/W): Serial transfer data. */
        case 0xff02:
            /* SC (R/W): SIO control. */
            /* Not implemented registers. */
            printf("ERROR: 0x%04x read not implemented!\n", addr);
            return 0;
        case 0xff04:
            /* DIV (R/W): Divider Register. */
            printf("ERROR: DIV not implemented!\n");
            return 0;
        case 0xff05:
            /* TIMA (R/W): Timer counter. */
            printf("ERROR: TIMA not implemented!\n");
            return 0;
        case 0xff06:
            /* TMA (R/W): Timer modulo. */
            printf("ERROR: TMA not implemented!\n");
            return 0;
        case 0xff07:
            /* TAC (R/W): Timer control. */
            printf("ERROR: TAC not implemented!\n");
            return 0;
        case 0xff0f:
            /* IF (R/W): Interrupt flag. */
            return interrupt_get_flag();
        case 0xff10:
        case 0xff11:
        case 0xff12:
        case 0xff13:
        case 0xff14:
        case 0xff16:
        case 0xff17:
        case 0xff18:
        case 0xff19:
        case 0xff1a:
        case 0xff1b:
        case 0xff1c:
        case 0xff1d:
        case 0xff1e:
        case 0xff20:
        case 0xff21:
        case 0xff22:
        case 0xff23:
        case 0xff24:
        case 0xff25:
        case 0xff26:
            printf("ERROR: sound register 0x%04x not implemented!\n", addr);
            return 0;
    }
    if (addr >= 0xff30 && addr < 0xff3f) {
        printf("ERROR: Wave pattern RAM not implemented.\n");
        return 0;
    } else if (addr == 0xff40) {
        return gpu_get_lcd_control();
    } else if (addr == 0xff41) {
        printf("ERROR: LCDC status not implemented.\n");
        return 0;
    } else if (addr == 0xff42) {
        return gpu_get_scroll_y();
    } else if (addr == 0xff43) {
        return gpu_get_scroll_x();
    } else if (addr == 0xff44) {
        return gpu_get_scanline();
    } else if (addr == 0xff45) {
        printf("ERROR: LY compare not implemented.\n");
        return 0;
    } else if (addr == 0xff46) {
        printf("ERROR: DMA not implemented.\n");
        return 0;
    } else if (addr == 0xff47) {
        printf("ERROR: BG & window palette read not implemented.\n");
        return 0;
    } else if (addr == 0xff48) {
        printf("ERROR: OBP0 read not implemented.\n");
        return 0;
    } else if (addr == 0xff49) {
        printf("ERROR: OBP1 read not implemented.\n");
        return 0;
    } else if (addr == 0xff4a) {
        printf("ERROR: WY not implemented.\n");
        return 0;
    } else if (addr == 0xff4b) {
        printf("ERROR: WX not implemented.\n");
        return 0;
    } else if (addr < 0xff80) {
        /* I/O ports. */
        return g_mmu.io[addr - 0xff00];
    } else if (addr < 0xffff) {
        /* Internal RAM. */
        return g_mmu.hram[addr - 0xff80];
    } else {
        /* Interrupt Enable Register. */
        ret = interrupt_get_enable();
        printf("IE: 0x%02x\n", ret);
        return ret;
    }
}

static void mmu_write_byte_ffxx(uint16_t addr, uint8_t value)
{
    /* I/O Registers. */
    switch (addr) {
        case 0xff00:
        /* P1 (R/W): Register for reading joy pad info. */
        case 0xff01:
        /* SB (R/W): Serial transfer data. */
        case 0xff02:
            /* SC (R/W): SIO control. */
            /* Not implemented registers. */
            printf("ERROR: 0x%04x write not implemented!\n", addr);
            return;
        case 0xff04:
            /* DIV (R/W): Divider Register. */
            printf("ERROR: DIV not implemented!\n");
            return;
        case 0xff05:
            /* TIMA (R/W): Timer counter. */
            printf("ERROR: TIMA not implemented!\n");
            return;
        case 0xff06:
            /* TMA (R/W): Timer modulo. */
            printf("ERROR: TMA not implemented!\n");
            return;
        case 0xff07:
            /* TAC (R/W): Timer control. */
            printf("ERROR: TAC not implemented!\n");
            return;
        case 0xff0f:
            /* IF (R/W): Interrupt flag. */
            interrupt_set_flag(value);
            return;
        case 0xff10:
        case 0xff11:
        case 0xff12:
        case 0xff13:
        case 0xff14:
        case 0xff16:
        case 0xff17:
        case 0xff18:
        case 0xff19:
        case 0xff1a:
        case 0xff1b:
        case 0xff1c:
        case 0xff1d:
        case 0xff1e:
        case 0xff20:
        case 0xff21:
        case 0xff22:
        case 0xff23:
        case 0xff24:
        case 0xff25:
        case 0xff26:
            printf("ERROR: sound register 0x%04x not implemented!\n", addr);
            return;
    }
    if (addr >= 0xff30 && addr < 0xff3f) {
        printf("ERROR: Wave pattern RAM not implemented.\n");
    } else if (addr == 0xff40) {
        gpu_set_lcd_control(value);
    } else if (addr == 0xff41) {
        printf("ERROR: LCDC status not implemented.\n");
    } else if (addr == 0xff42) {
        gpu_set_scroll_y(value);
    } else if (addr == 0xff43) {
        gpu_set_scroll_x(value);
    } else if (addr == 0xff44) {
        printf("ERROR: scanline not implemented.\n");
    } else if (addr == 0xff45) {
        printf("ERROR: LY compare not implemented.\n");
    } else if (addr == 0xff46) {
        printf("ERROR: DMA not implemented.\n");
    } else if (addr == 0xff47) {
        gpu_set_bg_palette(value);
    } else if (addr == 0xff48) {
        gpu_set_sprite_palette0(value);
    } else if (addr == 0xff49) {
        gpu_set_sprite_palette1(value);
    } else if (addr == 0xff4a) {
        printf("ERROR: WY not implemented.\n");
    } else if (addr == 0xff4b) {
        printf("ERROR: WX not implemented.\n");
    } else if (addr < 0xff80) {
        /* I/O ports. */
        printf("I0 W addr=%04x, i=%04x, value=%02x\n", addr, addr - 0xff00, value);
        g_mmu.io[addr - 0xff00] = value;
    } else if (addr < 0xffff) {
        /* Internal RAM. */
        g_mmu.hram[addr - 0xff80] = value;
    } else {
        /* Interrupt Enable Register. */
        interrupt_set_enable(value);
    }
}

/* Read 8-bit byte from a given address */
uint8_t mmu_read_byte(uint16_t addr)
{
    uint8_t ret;
    // printf("MMU: read byte 0x%04x\n", addr);
    switch (addr & 0xf000) {
        case 0x0000:
            /* 256 B Internal ROM accessed after reset. */
            if (g_mmu.read_internal_rom) {
                if (addr < 0x0100) {
                    ret = rom_read_internal(addr);
                    // printf("IROM: 0x%02x\n", ret);
                    return ret;
                } else if (addr == 0x0100) {
                    printf("switching to external ROM!\n");
                    g_mmu.read_internal_rom = false;
                }
            }
        /* Fall through! */

        /* 16kB ROM bank #0. */
        case 0x1000:
        case 0x2000:
        case 0x3000:

        /* 16kB Switchable ROM bank. */
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            ret = g_mmu.cart[addr];
            // printf("ROM: 0x%02x\n", ret);
            return ret;

        /* 8kB Video RAM. */
        case 0x8000:
        case 0x9000:
            ret = g_mmu.vram[addr & 0x1fff];
            // printf("VRAM: 0x%02x\n", ret);
            return ret;

        /* 8kB Switchable RAM bank. */
        case 0xa000:
        case 0xb000:
            ret = g_mmu.sram[addr & 0x1fff];
            // printf("SRAM: 0x%02x\n", ret);
            return ret;

        /* 8kB Internal RAM. */
        case 0xc000:
        case 0xd000:
            ret = g_mmu.wram[addr & 0x1fff];
            // printf("IRAM: 0x%02x\n", ret);
            return ret;

        case 0xe000:
        case 0xf000:
            /* Echo of 8kB Internal RAM; */
            if (addr < 0xfe00) {
                ret = g_mmu.wram[addr & 0x1fff];
                // printf("EIRAM: 0x%02x\n", ret);
                return ret;
            }
            switch (addr & 0x0f00) {
                case 0xe00:
                    /* Sprite Attrib Memory (OAM). */
                    if ((addr & 0xff) < 0xa0) {
                        ret = g_mmu.oam[addr & 0xff];
                        printf("OAM: 0x%02x\n", ret);
                        return ret;
                    } else {
                        /* Remaining bytes read as 0. */
                        printf("??: 0x00\n");
                        return 0;
                    }

                case 0xf00:
                    return mmu_read_byte_ffxx(addr);
            }
    }
    fprintf(stderr, "ERROR: shouldn't execute this!\n");
    return 0;
}

uint16_t mmu_read_word(uint16_t addr)
{
    uint16_t addrh = (uint16_t)(addr + 1);
    uint16_t ret = (uint16_t)(mmu_read_byte(addrh) << 8 | mmu_read_byte(addr));
    // printf("MMU: read word: 0x%04x: 0x%04x\n", addr, ret);
    return ret;
}

void mmu_write_byte(uint16_t addr, uint8_t value)
{
    // printf("MMU: write byte: addr=0x%04x: value=0x%02x\n", addr, value);
    switch (addr & 0xf000) {
        /* 16kB ROM bank #0. */
        case 0x0000:
        case 0x1000:
/* MBC1: Turn external RAM on. */
#if 0
            switch (g_mmu.cart_type) {
                case ROM_MBC1:
                    g_mmu.ram_on = ((value & 0x0f) == 0x0a) ? true : false;
                    break;
            }
#endif
            break;
        case 0x2000:
        case 0x3000:
/* MBC1: ROM bank switch. */
#if 0
            switch (g_mmu.cart_type) {
                case ROM_MBC1:
                    /* TODO */
                    printf("not implemented!");
                    break;
            }
#endif
            break;

        /* 16kB ROM bank #1. */
        case 0x4000:
        case 0x5000:
        case 0x6000:
        case 0x7000:
            //printf("ROM write not implemented!\n");
            break;

        /* 8kB Video RAM. */
        case 0x8000:
        case 0x9000:
            g_mmu.vram[addr & 0x1fff] = value;
            if (addr < 0x9800)
                gpu_update_tile(addr);
            break;

        /* 8kB Switchable RAM bank. */
        case 0xa000:
        case 0xb000:
            g_mmu.sram[addr & 0x1fff] = value;
            break;

        /* 8kB Internal RAM. */
        case 0xc000:
        case 0xd000:
            g_mmu.wram[addr & 0x1fff] = value;
            break;

        case 0xe000:
        case 0xf000:
            /* Echo of 8kB Internal RAM; */
            if (addr < 0xfe00) {
                g_mmu.wram[addr & 0x1fff] = value;
                break;
            }
            switch (addr & 0x0f00) {
                case 0xe00:
                    /* Sprite Attrib Memory (OAM). */
                    if ((addr & 0xff) < 0xa0) {
                        g_mmu.oam[addr & 0xff] = value;
                    }
                    break;

                case 0xf00:
                    mmu_write_byte_ffxx(addr, value);
                    break;
            }
    }
}

void mmu_write_word(uint16_t addr, uint16_t value)
{
    // printf("MMU: write byte: addr=0x%04x: value=0x%04x\n", addr, value);
    uint16_t addrh = (uint16_t)(addr + 1);
    mmu_write_byte(addr, (uint8_t)(value & 0x00ff));
    mmu_write_byte(addrh, (uint8_t)((value & 0xff00) >> 8));
}
