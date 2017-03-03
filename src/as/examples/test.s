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
ld hl,$9fff     ; $150
teste:
ld (hl-),a      ; $153
bit 7,h         ; $154
jr nz, teste    ; $156
halt
