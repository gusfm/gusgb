; Basic gbas label test

.include "common.s"

    ld hl,$9fff     ; $150
    jp label1       ; $153

label1:
    xor a           ; $156

label2:
    ld (hl-),a      ; $157
    bit 7,h         ; $158
    jr nz, label2   ; $15a

    halt            ; $15c
