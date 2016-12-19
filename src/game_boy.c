#include "game_boy.h"
#include "cpu.h"

int game_boy_init(const char *rom_path)
{
    return cpu_init(rom_path);
}

void game_boy_main()
{
    for (int i = 0; i < 10; ++i) {
        cpu_emulate_cycle();
    }
}
