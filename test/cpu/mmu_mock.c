#include "apu/apu.h"
#include "cartridge/cart.h"
#include "gpu.h"
#include "mmu.h"

int mmu_init(const char *rom_path)
{
    (void)rom_path;
    return 0;
}

void mmu_finish(void)
{
}

void mmu_reset(void)
{
}

uint8_t mmu_read_byte_dma(uint16_t addr)
{
    (void)addr;
    return 0;
}

uint8_t mmu_read_byte(uint16_t addr)
{
    (void)addr;
    return 0;
}

uint16_t mmu_read_word(uint16_t addr)
{
    (void)addr;
    return 0;
}

void mmu_write_byte_dma(uint16_t addr, uint8_t value)
{
    (void)addr;
    (void)value;
}

void mmu_write_byte(uint16_t addr, uint8_t value)
{
    (void)addr;
    (void)value;
}

void mmu_write_word(uint16_t addr, uint16_t value)
{
    (void)addr;
    (void)value;
}

void mmu_stop(void)
{
}

void mmu_dump(uint16_t addr, uint16_t offset)
{
    (void)addr;
    (void)offset;
}

/* cart */

bool cart_is_cgb(void)
{
    return true;
}

/* gpu */

void gpu_tick(unsigned int clock_step)
{
    (void)clock_step;
}

void gpu_dump(void)
{
}

/* apu */
void apu_tick(unsigned int clock_step)
{
    (void)clock_step;
}
