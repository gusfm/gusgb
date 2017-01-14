#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cart.h"
#include "gpu.h"
#include "interrupt.h"
#include "keys.h"
#include "timer.h"

mmu_t g_mmu;

int mmu_init(const char *rom_path)
{
    int ret = cart_load(rom_path);
    if (ret < 0) {
        return -1;
    }
    memset(g_mmu.eram, 0, sizeof(g_mmu.eram));
    memset(g_mmu.wram, 0, sizeof(g_mmu.wram));
    memset(g_mmu.zram, 0, sizeof(g_mmu.zram));
    g_mmu.read_ext_rom = 0;
    interrupt_init();
    keys_init();
    return 0;
}

static uint8_t mmu_read_byte_ffxx(uint16_t addr)
{
    if (addr == 0xffff) {
        /* Interrupt Enable Register. */
        return interrupt_get_enable();
    } else if (addr > 0xff7f) {
        /* Internal RAM. */
        return g_mmu.zram[addr & 0x007f];
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
            printf("NOTE: Serial transfer not implemented!\n");
            break;
        case 0xff02:
            /* SC (R/W): SIO control. */
            /* Not implemented registers. */
            printf("NOTE: Serial control not implemented!\n");
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
            printf("ERROR: sound register 0x%04x not implemented!\n", addr);
            return 0;
    }
    if (addr >= 0xff30 && addr <= 0xff3f) {
        printf("ERROR: Wave pattern RAM not implemented.\n");
        return 0;
    } else if (addr == 0xff50) {
        return g_mmu.read_ext_rom;
    } else if (addr >= 0xff40 && addr < 0xff80) {
        return gpu_read_byte(addr);
    } else {
        /* I/O ports. */
        printf("IO R addr=0x%04x, i=0x%04x, value=0x%02x\n", addr,
               addr - 0xff00, g_mmu.io[addr - 0xff00]);
        return g_mmu.io[addr - 0xff00];
    }
}

static void mmu_write_byte_ffxx(uint16_t addr, uint8_t value)
{
    if (addr == 0xffff) {
        /* Interrupt Enable Register. */
        interrupt_set_enable(value);
    } else if (addr > 0xff7f) {
        /* Internal RAM. */
        g_mmu.zram[addr & 0x007f] = value;
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
                printf("NOTE: Serial transfer not implemented!\n");
                break;
            case 0xff02:
                /* SC (R/W): SIO control. */
                /* Not implemented registers. */
                printf("NOTE: Serial control not implemented!\n");
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
                printf("ERROR: sound register 0x%04x not implemented!\n", addr);
                return;
        }
        if (addr >= 0xff30 && addr <= 0xff3f) {
            printf("ERROR: Wave pattern RAM not implemented.\n");
        } else if (addr == 0xff50) {
            printf("Switching to external ROM!\n");
            g_mmu.read_ext_rom = value;
        } else if (addr >= 0xff40 && addr < 0xff80) {
            gpu_write_byte(addr, value);
        } else {
            /* I/O ports. */
            printf("IO W addr=0x%04x, i=0x%04x, value=0x%02x\n", addr,
                   addr - 0xff00, value);
            g_mmu.io[addr - 0xff00] = value;
        }
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
            if (g_mmu.read_ext_rom == 0 && addr < 0x0100) {
                ret = read_internal_rom(addr);
                // printf("IROM: 0x%02x\n", ret);
                return ret;
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
            ret = cart_read_rom(addr);
            // printf("ROM: 0x%02x\n", ret);
            return ret;

        /* 8kB Video RAM. */
        case 0x8000:
        case 0x9000:
            return gpu_read_vram(addr);

        /* 8kB Switchable RAM bank. */
        case 0xa000:
        case 0xb000:
            ret = g_mmu.eram[addr & 0x1fff];
            // printf("SRAM: 0x%02x\n", ret);
            return ret;

        /* 8kB Internal RAM. */
        case 0xc000:
        case 0xd000:
            ret = g_mmu.wram[addr & 0x1fff];
            // printf("WRAM: 0x%04x 0x%02x\n", addr, ret);
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
                    if (addr < 0xfea0) {
                        return gpu_read_oam(addr);
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
            // printf("ROM write not implemented!\n");
            break;

        /* 8kB Video RAM. */
        case 0x8000:
        case 0x9000:
            gpu_write_vram(addr, value);
            break;

        /* 8kB Switchable RAM bank. */
        case 0xa000:
        case 0xb000:
            g_mmu.eram[addr & 0x1fff] = value;
            break;

        /* 8kB Internal RAM. */
        case 0xc000:
        case 0xd000:
            g_mmu.wram[addr & 0x1fff] = value;
            // printf("WRAM: 0x%04x 0x%02x\n", addr, value);
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
                    if (addr < 0xfea0) {
                        gpu_write_oam(addr, value);
                    }
                    /* Don't change 0xfea0 - 0xfeff. */
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
