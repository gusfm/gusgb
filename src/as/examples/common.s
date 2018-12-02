.fill $00, $8000        ; zero all 32k cartridge memory
.org $100
; start
nop
jp $150                 ; jump to code
; cartridge header
; assembler will auto fill header [$104 - $14f] with -g param
; code start at $150
.org $150
