.memset $00, $8000      ; zero all 32k cartridge memory
.seek $100
; start
nop
jp start                 ; jump to code
; cartridge header
; assembler will auto fill header [$104 - $14f] with -g param
; code start at $150
.seek $150
start:
    di                  ; Disabe interrupts
; Turn off LCD display
lcdoff:
    ld a,($ff00+$44)    ; $ff44=LCDC Y-Pos
    cp $90              ; $90 and bigger = in VBL
    jr nz,lcdoff        ; Loop until = $90
    xor a
    ld ($ff00+$40),a    ; turn off lcd so we can write to vram any time
; zero the memory from $8000-$9fff (vram)
    ld hl,$9fff
loop:
    ld (hl-),a
    bit 7,h
    jr nz, loop
; turn on lcd, showing background
    ld a,$91
    ld ($ff00+$40),a
    halt
