obj = src/cartridge/mbc1.o \
	  src/cartridge/mbc3.o \
	  src/cartridge/mbc5.o \
	  src/cartridge/cart.o \
	  src/clock.o \
	  src/interrupt.o \
	  src/timer.o \
	  src/gpu.o \
	  src/keys.o \
	  src/apu/timer.o \
	  src/apu/length_counter.o \
	  src/apu/volume_envelope.o \
	  src/apu/sqr_ch.o \
	  src/apu/wave_ch.o \
	  src/apu/noise_ch.o \
	  src/apu/apu.o \
	  src/mmu.o \
	  src/cpu_utils.o \
	  src/cpu_opcodes.o \
	  src/cpu_ext_ops.o \
	  src/cpu.o \
	  src/game_boy.o \
	  src/main.o

dep = $(obj:.o=.d)

CFLAGS = -Wall -Wextra -std=gnu11 -O2
CPPFLAGS = -I./src
LDFLAGS = -lSDL2

gusgb: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f gusgb $(obj) $(dep)
