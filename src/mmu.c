#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cart.h"
#include "gpu.h"
#include "interrupt.h"
#include "keys.h"
#include "timer.h"

const uint8_t g_internal_rom[0x100] = {
    0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb,
    0x21, 0x26, 0xff, 0x0e, 0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
    0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0, 0x47, 0x11, 0x04, 0x01,
    0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
    0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22,
    0x23, 0x05, 0x20, 0xf9, 0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
    0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20, 0xf9, 0x2e, 0x0f, 0x18,
    0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
    0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20,
    0xf7, 0x1d, 0x20, 0xf2, 0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
    0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06, 0x7b, 0xe2, 0x0c, 0x3e,
    0x87, 0xf2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
    0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17,
    0xc1, 0xcb, 0x11, 0x17, 0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
    0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,
    0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x4c, 0x21, 0x04, 0x01, 0x11,
    0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
    0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe,
    0x3e, 0x01, 0xe0, 0x50};

mmu_t MMU;

int mmu_init(const char *rom_path)
{
    int ret = cart_load(rom_path);
    if (ret < 0) {
        return -1;
    }
    mmu_reset();
    return 0;
}

void mmu_reset(void)
{
    memset(MMU.wram, 0, sizeof(MMU.wram));
    memset(MMU.zram, 0, sizeof(MMU.zram));
    MMU.read_ext_rom = 0;
    interrupt_init();
    keys_init();
}

static uint8_t mmu_read_byte_ffxx(uint16_t addr)
{
    if (addr == 0xffff) {
        /* Interrupt Enable Register. */
        return interrupt_get_enable();
    } else if (addr > 0xff7f) {
        /* Internal RAM. */
        return MMU.zram[addr & 0x007f];
    } else if (addr >= 0xff04 && addr <= 0xff07) {
        /* Timer. */
        return timer_read_byte(addr);
    }
    /* I/O Registers. */
    switch (addr) {
        case 0xff00:
            /* P1 (R/W): Register for reading joy pad info. */
            return keys_read();
        case 0xff01:
            /* SB (R/W): Serial transfer data. */
            break;
        case 0xff02:
            /* SC (R/W): SIO control. */
            /* Not implemented registers. */
            break;
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
            return 0;
    }
    if (addr >= 0xff30 && addr <= 0xff3f) {
        return 0;
    } else if (addr == 0xff50) {
        return MMU.read_ext_rom;
    } else if (addr >= 0xff40 && addr < 0xff80) {
        return gpu_read_byte(addr);
    } else {
        /* I/O ports. */
        printf("IO R addr=0x%04x, i=0x%04x, value=0x%02x\n", addr,
               addr - 0xff00, MMU.io[addr - 0xff00]);
        return MMU.io[addr - 0xff00];
    }
}

static void mmu_write_byte_ffxx(uint16_t addr, uint8_t value)
{
    if (addr == 0xffff) {
        /* Interrupt Enable Register. */
        interrupt_set_enable(value);
    } else if (addr > 0xff7f) {
        /* Internal RAM. */
        MMU.zram[addr & 0x007f] = value;
    } else if (addr >= 0xff04 && addr <= 0xff07) {
        /* Timer. */
        timer_write_byte(addr, value);
    } else {
        /* I/O Registers. */
        switch (addr) {
            case 0xff00:
                /* P1 (R/W): Register for reading joy pad info. */
                keys_write(value);
                return;
            case 0xff01:
                /* SB (R/W): Serial transfer data. */
                break;
            case 0xff02:
                /* SC (R/W): SIO control. */
                /* Not implemented registers. */
                break;
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
                return;
        }
        if (addr >= 0xff30 && addr <= 0xff3f) {
            return;
        } else if (addr == 0xff50) {
            MMU.read_ext_rom = value;
        } else if (addr >= 0xff40 && addr < 0xff80) {
            gpu_write_byte(addr, value);
        } else {
            /* I/O ports. */
            printf("IO W addr=0x%04x, i=0x%04x, value=0x%02x\n", addr,
                   addr - 0xff00, value);
            MMU.io[addr - 0xff00] = value;
        }
    }
}

/* Read 8-bit byte from a given address */
uint8_t mmu_read_byte(uint16_t addr)
{
    if (addr < 0x4000) {
        /* 256B Internal ROM accessed after reset. */
        if (MMU.read_ext_rom == 0 && addr < 0x0100) {
            if (addr == 0xff) {
                /* Last internal rom access. Enable rendering if disabled. */
                gpu_gl_enable();
            }
            return g_internal_rom[addr];
        }
        /* 16kB ROM bank 0. */
        return cart_read_rom0(addr);
    } else if (addr < 0x8000) {
        /* 16kB switchable ROM bank. */
        return cart_read_rom1(addr);
    } else if (addr < 0xa000) {
        /* 8kB Video RAM. */
        return gpu_read_vram(addr);
    } else if (addr < 0xc000) {
        /* 8kB Switchable RAM bank. */
        return cart_read_ram(addr);
    } else if (addr < 0xe000) {
        /* 8kB Internal RAM. */
        return MMU.wram[addr & 0x1fff];
    } else if (addr < 0xfe00) {
        /* Echo of 8kB Internal RAM; */
        return MMU.wram[addr & 0x1fff];
    } else if (addr < 0xff00) {
        /* Sprite Attrib Memory (OAM). */
        if (addr < 0xfea0) {
            return gpu_read_oam(addr);
        } else {
            /* Remaining bytes read as 0. */
            printf("??: 0x00\n");
            return 0;
        }
    } else {
        return mmu_read_byte_ffxx(addr);
    }
}

uint16_t mmu_read_word(uint16_t addr)
{
    uint16_t addrh = (uint16_t)(addr + 1);
    return (uint16_t)(mmu_read_byte(addrh) << 8 | mmu_read_byte(addr));
}

void mmu_write_byte(uint16_t addr, uint8_t value)
{
    if (addr < 0x8000) {
        /* 16kB ROM bank 0 and 16kB switchable ROM bank. */
        cart_write_mbc(addr, value);
    } else if (addr < 0xa000) {
        /* 8kB Video RAM. */
        gpu_write_vram(addr, value);
    } else if (addr < 0xc000) {
        /* 8kB Switchable RAM bank. */
        cart_write_ram(addr, value);
    } else if (addr < 0xe000) {
        /* 8kB Internal RAM. */
        MMU.wram[addr & 0x1fff] = value;
    } else if (addr < 0xfe00) {
        /* Echo of 8kB Internal RAM; */
        MMU.wram[addr & 0x1fff] = value;
    } else if (addr < 0xff00) {
        /* Sprite Attrib Memory (OAM). */
        if (addr < 0xfea0) {
            gpu_write_oam(addr, value);
        }
        /* Don't change 0xfea0 - 0xfeff. */
    } else {
        mmu_write_byte_ffxx(addr, value);
    }
}

void mmu_write_word(uint16_t addr, uint16_t value)
{
    uint16_t addrh = (uint16_t)(addr + 1);
    mmu_write_byte(addr, (uint8_t)(value & 0x00ff));
    mmu_write_byte(addrh, (uint8_t)((value & 0xff00) >> 8));
}
