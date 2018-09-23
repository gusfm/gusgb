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
	  src/gusgb.o \
	  src/main.o

# Debugger option
DEBUGGER ?= n
ifeq ($(DEBUGGER),y)
obj += src/debugger/button.o
obj += src/debugger/text.o
obj += src/debugger/palette_window.o
obj += src/debugger/bg_map_window.o
obj += src/debugger/tile_window.o
obj += src/debugger/debugger.o
FLAGS = -DDEBUGGER
LDFLAGS += -lSDL2_ttf
endif

dep = $(obj:.o=.d)

CFLAGS = -Wall -Wextra -std=gnu11 -O2 -fno-strict-aliasing $(FLAGS)
CPPFLAGS = -I./src
LDFLAGS += -lSDL2

gusgb: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)

%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f gusgb $(obj) $(dep)
