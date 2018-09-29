#include "mmu.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apu/apu.h"
#include "cartridge/cart.h"
#include "clock.h"
#include "gpu.h"
#include "interrupt.h"
#include "keys.h"
#include "timer.h"

static mmu_t MMU;

int mmu_init(const char *rom_path)
{
    int ret = cart_load(rom_path);
    if (ret < 0) {
        return -1;
    }
    mmu_reset();
    return 0;
}

void mmu_finish(void)
{
    cart_unload();
}

void mmu_reset(void)
{
    memset(MMU.wram, 0, sizeof(MMU.wram));
    memset(MMU.zram, 0, sizeof(MMU.zram));
    if (cart_is_cgb()) {
        MMU.speed_switch = 0x7e;
    } else {
        MMU.speed_switch = 0xff;
        MMU.hdma1 = 0xff;
        MMU.hdma2 = 0xff;
        MMU.hdma3 = 0xff;
        MMU.hdma4 = 0xff;
        MMU.hdma5 = 0xff;
        MMU.ir = 0xff;
    }
    MMU.wram_bank = 0;
    interrupt_reset();
    keys_reset();
    apu_reset();
    gpu_reset();
}

static void mmu_dma_start(uint8_t mode)
{
    uint16_t src = ((MMU.hdma1 << 8) | MMU.hdma2) & 0xfff0;
    uint16_t dest = ((MMU.hdma3 << 8) | MMU.hdma4) & 0xfff0;
    if (mode & 0x80) {
        /* H-Blank DMA */
        printf("%s: not implemented\n", __func__);
    } else {
        /* General Purpose DMA */
        unsigned int length = (mode & 0x7f) + 1;
        for (unsigned int i = 0; i < length; ++i) {
            for (unsigned int j = 0; j < 16; ++j) {
                mmu_write_byte_dma(dest++, mmu_read_byte_dma(src++));
            }
            clock_step(32 << MMU.clock_speed);
        }
        MMU.hdma5 = 0xff;
    }
}

static uint8_t mmu_read_reg(uint16_t addr)
{
    switch (addr & 0x007f) {
        case 0x00:
            return keys_read();
        case 0x01:
            /* TODO: SB Serial transfer data */
            return 0;
        case 0x02:
            /* TODO: SIO control */
            return 0x7e;
        case 0x04:
            return timer_read_div();
        case 0x05:
            return timer_read_tima();
        case 0x06:
            return timer_read_tma();
        case 0x07:
            return timer_read_tac();
        case 0x0f:
            return interrupt_get_flag();
        case 0x10:
            return apu_read_nr10();
        case 0x11:
            return apu_read_nr11();
        case 0x12:
            return apu_read_nr12();
        case 0x13:
            return apu_read_nr13();
        case 0x14:
            return apu_read_nr14();
        case 0x15:
            return 0xff;
        case 0x16:
            return apu_read_nr21();
        case 0x17:
            return apu_read_nr22();
        case 0x18:
            return apu_read_nr23();
        case 0x19:
            return apu_read_nr24();
        case 0x1a:
            return apu_read_nr30();
        case 0x1b:
            return apu_read_nr31();
        case 0x1c:
            return apu_read_nr32();
        case 0x1d:
            return apu_read_nr33();
        case 0x1e:
            return apu_read_nr34();
        case 0x1f:
            return 0xff;
        case 0x20:
            return apu_read_nr41();
        case 0x21:
            return apu_read_nr42();
        case 0x22:
            return apu_read_nr43();
        case 0x23:
            return apu_read_nr44();
        case 0x24:
            return apu_read_nr50();
        case 0x25:
            return apu_read_nr51();
        case 0x26:
            return apu_read_nr52();
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            return 0xff;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
            return apu_read_wave(addr & 0xf);
        case 0x40:
            return gpu_read_lcdc();
        case 0x41:
            return gpu_read_stat();
        case 0x42:
            return gpu_read_scy();
        case 0x43:
            return gpu_read_scx();
        case 0x44:
            return gpu_read_ly();
        case 0x45:
            return gpu_read_lyc();
        case 0x46:
            return gpu_read_dma();
        case 0x47:
            return gpu_read_bgp();
        case 0x48:
            return gpu_read_obp0();
        case 0x49:
            return gpu_read_obp1();
        case 0x4a:
            return gpu_read_wy();
        case 0x4b:
            return gpu_read_wx();
        case 0x4d:
            return MMU.speed_switch;
        case 0x4f:
            return gpu_read_vbk();
        case 0x50:
            return 0xff;
        case 0x51:
            return MMU.hdma1;
        case 0x52:
            return MMU.hdma2;
        case 0x53:
            return MMU.hdma3;
        case 0x54:
            return MMU.hdma4;
        case 0x55:
            return MMU.hdma5;
        case 0x56:
            return MMU.ir;
        case 0x68:
            return gpu_read_bgpi();
        case 0x69:
            return gpu_read_bgpd();
        case 0x6a:
            return gpu_read_obpi();
        case 0x6b:
            return gpu_read_obpd();
        case 0x70:
            if (cart_is_cgb())
                return MMU.wram_bank;
            return 0xff;
        default:
            return 0xff;
    }
}

static void mmu_write_reg(uint16_t addr, uint8_t value)
{
    switch (addr & 0x007f) {
        case 0x00:
            keys_write(value);
            break;
        case 0x01:
            /* TODO: SB Serial transfer data */
            break;
        case 0x02:
            /* TODO: SIO control */
            break;
        case 0x04:
            timer_write_div();
            break;
        case 0x05:
            timer_write_tima(value);
            break;
        case 0x06:
            timer_write_tma(value);
            break;
        case 0x07:
            timer_write_tac(value);
            break;
        case 0x0f:
            interrupt_set_flag(value);
            break;
        case 0x10:
            apu_write_nr10(value);
            break;
        case 0x11:
            apu_write_nr11(value);
            break;
        case 0x12:
            apu_write_nr12(value);
            break;
        case 0x13:
            apu_write_nr13(value);
            break;
        case 0x14:
            apu_write_nr14(value);
            break;
        case 0x15:
            break;
        case 0x16:
            apu_write_nr21(value);
            break;
        case 0x17:
            apu_write_nr22(value);
            break;
        case 0x18:
            apu_write_nr23(value);
            break;
        case 0x19:
            apu_write_nr24(value);
            break;
        case 0x1a:
            apu_write_nr30(value);
            break;
        case 0x1b:
            apu_write_nr31(value);
            break;
        case 0x1c:
            apu_write_nr32(value);
            break;
        case 0x1d:
            apu_write_nr33(value);
            break;
        case 0x1e:
            apu_write_nr34(value);
            break;
        case 0x1f:
            break;
        case 0x20:
            apu_write_nr41(value);
            break;
        case 0x21:
            apu_write_nr42(value);
            break;
        case 0x22:
            apu_write_nr43(value);
            break;
        case 0x23:
            apu_write_nr44(value);
            break;
        case 0x24:
            apu_write_nr50(value);
            break;
        case 0x25:
            apu_write_nr51(value);
            break;
        case 0x26:
            apu_write_nr52(value);
            break;
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            break;
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
            apu_write_wave(addr & 0xf, value);
            break;
        case 0x40:
            gpu_write_lcdc(value);
            break;
        case 0x41:
            gpu_write_stat(value);
            break;
        case 0x42:
            gpu_write_scy(value);
            break;
        case 0x43:
            gpu_write_scx(value);
            break;
        case 0x44:
            break;
        case 0x45:
            gpu_write_lyc(value);
            break;
        case 0x46:
            gpu_write_dma(value);
            break;
        case 0x47:
            gpu_write_bgp(value);
            break;
        case 0x48:
            gpu_write_obp0(value);
            break;
        case 0x49:
            gpu_write_obp1(value);
            break;
        case 0x4a:
            gpu_write_wy(value);
            break;
        case 0x4b:
            gpu_write_wx(value);
            break;
        case 0x4d:
            if (cart_is_cgb())
                MMU.speed_switch = (MMU.speed_switch & 0xfe) | (value & 1);
            break;
        case 0x4f:
            gpu_write_vbk(value);
            break;
        case 0x50:
            /* Disable internal ROM. */
            break;
        case 0x51:
            if (cart_is_cgb())
                MMU.hdma1 = value;
            break;
        case 0x52:
            if (cart_is_cgb())
                MMU.hdma2 = value;
            break;
        case 0x53:
            if (cart_is_cgb())
                MMU.hdma3 = value;
            break;
        case 0x54:
            if (cart_is_cgb())
                MMU.hdma4 = value;
            break;
        case 0x55:
            if (cart_is_cgb()) {
                mmu_dma_start(value);
            }
            break;
        case 0x56:
            if (cart_is_cgb())
                MMU.ir = value;
            break;
        case 0x68:
            gpu_write_bgpi(value);
            break;
        case 0x69:
            gpu_write_bgpd(value);
            break;
        case 0x6a:
            gpu_write_obpi(value);
            break;
        case 0x6b:
            gpu_write_obpd(value);
            break;
        case 0x70:
            if (cart_is_cgb())
                MMU.wram_bank = value & 7;
            break;
        default:
            break;
    }
}

static uint8_t wram_get_bank(void)
{
    if (MMU.wram_bank == 0)
        return 1;
    return MMU.wram_bank;
}

uint8_t mmu_read_byte_dma(uint16_t addr)
{
    if (addr < 0x4000) {
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
    } else if (addr < 0xd000) {
        /* 4KB Work RAM Bank 0 (WRAM). */
        return MMU.wram[0][addr & 0x0fff];
    } else if (addr < 0xe000) {
        /* 4KB Work RAM Bank 1 (Switchable WRAM). */
        return MMU.wram[wram_get_bank()][addr & 0x0fff];
    } else if (addr < 0xfe00) {
        /* Echo of 8kB Internal RAM. */
        return MMU.wram[0][addr & 0x1fff];
    } else if (addr < 0xff00) {
        /* Sprite Attrib Memory (OAM). */
        if (addr < 0xfea0) {
            return gpu_read_oam(addr);
        } else if (addr < 0xff00) {
            /* Empty. */
            return 0;
        }
    } else if (addr < 0xff80) {
        /* I/O Registers. */
        return mmu_read_reg(addr);
    } else if (addr < 0xffff) {
        /* Internal RAM. */
        return MMU.zram[addr & 0x007f];
    } else {
        /* Interrupt Enable Register. */
        return interrupt_get_enable();
    }
    abort();
}

uint8_t mmu_read_byte(uint16_t addr)
{
    clock_step(4);
    return mmu_read_byte_dma(addr);
}

uint16_t mmu_read_word(uint16_t addr)
{
    uint16_t addrh = (uint16_t)(addr + 1);
    return (uint16_t)(mmu_read_byte(addrh) << 8 | mmu_read_byte(addr));
}

void mmu_write_byte_dma(uint16_t addr, uint8_t value)
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
    } else if (addr < 0xd000) {
        /* 4KB Work RAM Bank 0 (WRAM). */
        MMU.wram[0][addr & 0x0fff] = value;
    } else if (addr < 0xe000) {
        /* 4KB Work RAM Bank 1 (Switchable WRAM). */
        MMU.wram[wram_get_bank()][addr & 0x0fff] = value;
    } else if (addr < 0xfe00) {
        /* 4KB Work RAM Bank 0 (WRAM). */
        MMU.wram[0][addr & 0x1fff] = value;
    } else if (addr < 0xff00) {
        /* Sprite Attrib Memory (OAM). */
        if (addr < 0xfea0) {
            gpu_write_oam(addr, value);
        }
        /* Don't change 0xfea0 - 0xfeff. */
    } else if (addr < 0xff80) {
        /* I/O Registers. */
        mmu_write_reg(addr, value);
    } else if (addr < 0xffff) {
        /* Internal RAM. */
        MMU.zram[addr & 0x007f] = value;
    } else {
        /* Interrupt Enable Register. */
        interrupt_set_enable(value);
    }
}

void mmu_write_byte(uint16_t addr, uint8_t value)
{
    clock_step(4);
    mmu_write_byte_dma(addr, value);
}

void mmu_write_word(uint16_t addr, uint16_t value)
{
    uint16_t addrh = (uint16_t)(addr + 1);
    mmu_write_byte(addr, (uint8_t)(value & 0x00ff));
    mmu_write_byte(addrh, (uint8_t)((value & 0xff00) >> 8));
}

void mmu_stop(void)
{
    if (cart_is_cgb() && MMU.speed_switch & 1) {
        MMU.speed_switch = ((~MMU.speed_switch) & 0x80) | 0x7e;
        MMU.clock_speed = MMU.speed_switch >> 7;
        gpu_change_speed(MMU.clock_speed);
        apu_change_speed(MMU.clock_speed);
    }
}

void mmu_dump(uint16_t addr, uint16_t offset)
{
    addr = addr - (addr % 16);
    uint16_t end = addr + offset;
    printf("MMU dump:");
    while (addr < end) {
        if (addr % 16 == 0) {
            printf("\n%.2hx: ", addr);
        }
        printf("%.2hhx ", mmu_read_byte(addr++));
    }
    printf("\n");
}
