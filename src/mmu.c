#include "mmu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interrupt.h"
#include "utils.h"
#include "rom.h"

mmu_t g_mmu;

int mmu_init(const char *rom_path)
{
    int ret = rom_load(rom_path, g_mmu.rom, sizeof(g_mmu.rom));
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
    interrupt_init();
    return 0;
}

/* Read 8-bit byte from a given address */
uint8_t mmu_read_byte(uint16_t addr)
{
    uint8_t ret;
    PRINTD("MMU: read byte 0x%04x: ", addr);
    switch (addr & 0xf000) {
        case 0x0000:
            /* 256 B Internal ROM accessed after reset. */
            if (g_mmu.read_internal_rom) {
                if (addr < 0x0100) {
                    ret = rom_read_internal(addr);
                    PRINTD("IROM: 0x%02x\n", ret);
                    return ret;
                } else if (addr == 0x0100) {
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
            ret = g_mmu.rom[addr];
            PRINTD("ROM: 0x%02x\n", ret);
            return ret;

        /* 8kB Video RAM. */
        case 0x8000:
        case 0x9000:
            ret = g_mmu.vram[addr & 0x1fff];
            PRINTD("VRAM: 0x%02x\n", ret);
            return ret;

        /* 8kB Switchable RAM bank. */
        case 0xa000:
        case 0xb000:
            ret = g_mmu.sram[addr & 0x1fff];
            PRINTD("SRAM: 0x%02x\n", ret);
            return ret;

        /* 8kB Internal RAM. */
        case 0xc000:
        case 0xd000:
            ret = g_mmu.wram[addr & 0x1fff];
            PRINTD("IRAM: 0x%02x\n", ret);
            return ret;

        case 0xe000:
        case 0xf000:
            /* Echo of 8kB Internal RAM; */
            if (addr < 0xfe00) {
                ret = g_mmu.wram[addr & 0x1fff];
                PRINTD("EIRAM: 0x%02x\n", ret);
                return ret;
            }
            switch (addr & 0x0f00) {
                case 0xe00:
                    /* Sprite Attrib Memory (OAM). */
                    if ((addr & 0xff) < 0xa0) {
                        ret = g_mmu.oam[addr & 0xff];
                        PRINTD("OAM: 0x%02x\n", ret);
                        return ret;
                    } else {
                        /* Remaining bytes read as 0. */
                        PRINTD("??: 0x00\n");
                        return 0;
                    }

                case 0xf00:
                    if (addr < 0xff80) {
                        /* I/O ports. */
                        fprintf(stderr, "ERROR: I/O not implemented!\n");
                        // return g_mmu.io[addr & 0x7f];
                        return 0;
                    } else if (addr < 0xfffe) {
                        /* Internal RAM. */
                        ret = g_mmu.hram[addr - 0xff80];
                        PRINTD("HRAM: 0x%02x\n", ret);
                        return ret;
                    } else {
                        /* Interrupt Enable Register. */
                        ret = interrupt_get_enable();
                        PRINTD("IE: 0x%02x\n", ret);
                        return ret;
                    }
            }
    }
    fprintf(stderr, "ERROR: shouldn't execute this!\n");
    return 0;
}

uint16_t mmu_read_word(uint16_t addr)
{
    uint16_t ret = mmu_read_byte(addr + 1) << 8 | mmu_read_byte(addr);
    printf("MMU: read word: 0x%04x: 0x%04x\n", addr, ret);
    return ret;
}

void mmu_write_byte(uint16_t addr, uint8_t value)
{
    PRINTD("MMU: write byte: addr=0x%04x: value=0x%02x\n", addr, value);
}

void mmu_write_word(uint16_t addr, uint16_t value)
{
    PRINTD("MMU: write byte: addr=0x%04x: value=0x%04x\n", addr, value);
}
