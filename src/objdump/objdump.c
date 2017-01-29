#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *asm1;
    const char *asm2;
    uint8_t length;
} instruction_t;

const instruction_t instr[256] = {
    {"nop", NULL, 1},                // 0x00
    {"ld bc, $", NULL, 3},           // 0x01
    {"ld (bc), a", NULL, 1},         // 0x02
    {"inc bc", NULL, 1},             // 0x03
    {"inc b", NULL, 1},              // 0x04
    {"dec b", NULL, 1},              // 0x05
    {"ld b, $", NULL, 2},            // 0x06
    {"rlca", NULL, 1},               // 0x07
    {"ld ($", "), sp", 3},           // 0x08
    {"add hl, bc", NULL, 1},         // 0x09
    {"ld a, (bc)", NULL, 1},         // 0x0a
    {"dec bc", NULL, 1},             // 0x0b
    {"inc c", NULL, 1},              // 0x0c
    {"dec c", NULL, 1},              // 0x0d
    {"ld c, $", NULL, 2},            // 0x0e
    {"rrca", NULL, 1},               // 0x0f
    {"stop", NULL, 1},               // 0x10
    {"ld de, $", NULL, 3},           // 0x11
    {"ld (de), a", NULL, 1},         // 0x12
    {"inc de", NULL, 1},             // 0x13
    {"inc d", NULL, 1},              // 0x14
    {"dec d", NULL, 1},              // 0x15
    {"ld d, $", NULL, 2},            // 0x16
    {"rla", NULL, 1},                // 0x17
    {"jr $", NULL, 2},               // 0x18
    {"add hl, de", NULL, 1},         // 0x19
    {"ld a, (de)", NULL, 1},         // 0x1a
    {"dec de", NULL, 1},             // 0x1b
    {"inc e", NULL, 1},              // 0x1c
    {"dec e", NULL, 1},              // 0x1d
    {"ld e, $", NULL, 2},            // 0x1e
    {"rra", NULL, 1},                // 0x1f
    {"jr nz, $", NULL, 2},           // 0x20
    {"ld hl, $", NULL, 3},           // 0x21
    {"ld (hl+), a", NULL, 1},        // 0x22
    {"inc hl", NULL, 1},             // 0x23
    {"inc h", NULL, 1},              // 0x24
    {"dec h", NULL, 1},              // 0x25
    {"ld h, $", NULL, 2},            // 0x26
    {"daa", NULL, 1},                // 0x27
    {"jr z, $", NULL, 2},            // 0x28
    {"add hl, hl", NULL, 1},         // 0x29
    {"ld a, (hl+)", NULL, 1},        // 0x2a
    {"dec hl", NULL, 1},             // 0x2b
    {"inc l", NULL, 1},              // 0x2c
    {"dec l", NULL, 1},              // 0x2d
    {"ld l, $", NULL, 2},            // 0x2e
    {"cpl", NULL, 1},                // 0x2f
    {"jr nc, $", NULL, 2},           // 0x30
    {"ld sp, $", NULL, 3},           // 0x31
    {"ld (hl-), a", NULL, 1},        // 0x32
    {"inc sp", NULL, 1},             // 0x33
    {"inc (hl)", NULL, 1},           // 0x34
    {"dec (hl)", NULL, 1},           // 0x35
    {"ld (hl), $", NULL, 2},         // 0x36
    {"scf", NULL, 1},                // 0x37
    {"jr c, $", NULL, 2},            // 0x38
    {"add hl, sp", NULL, 1},         // 0x39
    {"ld a, (hl-)", NULL, 1},        // 0x3a
    {"dec sp", NULL, 1},             // 0x3b
    {"inc a", NULL, 1},              // 0x3c
    {"dec a", NULL, 1},              // 0x3d
    {"ld a, $", NULL, 2},            // 0x3e
    {"ccf", NULL, 1},                // 0x3f
    {"ld b, b", NULL, 1},            // 0x40
    {"ld b, c", NULL, 1},            // 0x41
    {"ld b, d", NULL, 1},            // 0x42
    {"ld b, e", NULL, 1},            // 0x43
    {"ld b, h", NULL, 1},            // 0x44
    {"ld b, l", NULL, 1},            // 0x45
    {"ld b, (hl)", NULL, 1},         // 0x46
    {"ld b, a", NULL, 1},            // 0x47
    {"ld c, b", NULL, 1},            // 0x48
    {"ld c, c", NULL, 1},            // 0x49
    {"ld c, d", NULL, 1},            // 0x4a
    {"ld c, e", NULL, 1},            // 0x4b
    {"ld c, h", NULL, 1},            // 0x4c
    {"ld c, l", NULL, 1},            // 0x4d
    {"ld c, (hl)", NULL, 1},         // 0x4e
    {"ld c, a", NULL, 1},            // 0x4f
    {"ld d, b", NULL, 1},            // 0x50
    {"ld d, c", NULL, 1},            // 0x51
    {"ld d, d", NULL, 1},            // 0x52
    {"ld d, e", NULL, 1},            // 0x53
    {"ld d, h", NULL, 1},            // 0x54
    {"ld d, l", NULL, 1},            // 0x55
    {"ld d, (hl)", NULL, 1},         // 0x56
    {"ld d, a", NULL, 1},            // 0x57
    {"ld e, b", NULL, 1},            // 0x58
    {"ld e, c", NULL, 1},            // 0x59
    {"ld e, d", NULL, 1},            // 0x5a
    {"ld e, e", NULL, 1},            // 0x5b
    {"ld e, h", NULL, 1},            // 0x5c
    {"ld e, l", NULL, 1},            // 0x5d
    {"ld e, (hl)", NULL, 1},         // 0x5e
    {"ld e, a", NULL, 1},            // 0x5f
    {"ld h, b", NULL, 1},            // 0x60
    {"ld h, c", NULL, 1},            // 0x61
    {"ld h, d", NULL, 1},            // 0x62
    {"ld h, e", NULL, 1},            // 0x63
    {"ld h, h", NULL, 1},            // 0x64
    {"ld h, l", NULL, 1},            // 0x65
    {"ld h, (hl)", NULL, 1},         // 0x66
    {"ld h, a", NULL, 1},            // 0x67
    {"ld l, b", NULL, 1},            // 0x68
    {"ld l, c", NULL, 1},            // 0x69
    {"ld l, d", NULL, 1},            // 0x6a
    {"ld l, e", NULL, 1},            // 0x6b
    {"ld l, h", NULL, 1},            // 0x6c
    {"ld l, l", NULL, 1},            // 0x6d
    {"ld l, (hl)", NULL, 1},         // 0x6e
    {"ld l, a", NULL, 1},            // 0x6f
    {"ld (hl), b", NULL, 1},         // 0x70
    {"ld (hl), c", NULL, 1},         // 0x71
    {"ld (hl), d", NULL, 1},         // 0x72
    {"ld (hl), e", NULL, 1},         // 0x73
    {"ld (hl), h", NULL, 1},         // 0x74
    {"ld (hl), l", NULL, 1},         // 0x75
    {"halt", NULL, 1},               // 0x76
    {"ld (hl), a", NULL, 1},         // 0x77
    {"ld a, b", NULL, 1},            // 0x78
    {"ld a, c", NULL, 1},            // 0x79
    {"ld a, d", NULL, 1},            // 0x7a
    {"ld a, e", NULL, 1},            // 0x7b
    {"ld a, h", NULL, 1},            // 0x7c
    {"ld a, l", NULL, 1},            // 0x7d
    {"ld a, (hl)", NULL, 1},         // 0x7e
    {"ld a, a", NULL, 1},            // 0x7f
    {"add a, b", NULL, 1},           // 0x80
    {"add a, c", NULL, 1},           // 0x81
    {"add a, d", NULL, 1},           // 0x82
    {"add a, e", NULL, 1},           // 0x83
    {"add a, h", NULL, 1},           // 0x84
    {"add a, l", NULL, 1},           // 0x85
    {"add a, (hl)", NULL, 1},        // 0x86
    {"add a, a", NULL, 1},           // 0x87
    {"adc b", NULL, 1},              // 0x88
    {"adc c", NULL, 1},              // 0x89
    {"adc d", NULL, 1},              // 0x8a
    {"adc e", NULL, 1},              // 0x8b
    {"adc h", NULL, 1},              // 0x8c
    {"adc l", NULL, 1},              // 0x8d
    {"adc (hl)", NULL, 1},           // 0x8e
    {"adc a", NULL, 1},              // 0x8f
    {"sub b", NULL, 1},              // 0x90
    {"sub c", NULL, 1},              // 0x91
    {"sub d", NULL, 1},              // 0x92
    {"sub e", NULL, 1},              // 0x93
    {"sub h", NULL, 1},              // 0x94
    {"sub l", NULL, 1},              // 0x95
    {"sub (hl)", NULL, 1},           // 0x96
    {"sub a", NULL, 1},              // 0x97
    {"sbc b", NULL, 1},              // 0x98
    {"sbc c", NULL, 1},              // 0x99
    {"sbc d", NULL, 1},              // 0x9a
    {"sbc e", NULL, 1},              // 0x9b
    {"sbc h", NULL, 1},              // 0x9c
    {"sbc l", NULL, 1},              // 0x9d
    {"sbc (hl)", NULL, 1},           // 0x9e
    {"sbc a", NULL, 1},              // 0x9f
    {"and b", NULL, 1},              // 0xa0
    {"and c", NULL, 1},              // 0xa1
    {"and d", NULL, 1},              // 0xa2
    {"and e", NULL, 1},              // 0xa3
    {"and h", NULL, 1},              // 0xa4
    {"and l", NULL, 1},              // 0xa5
    {"and (hl)", NULL, 1},           // 0xa6
    {"and a", NULL, 1},              // 0xa7
    {"xor b", NULL, 1},              // 0xa8
    {"xor c", NULL, 1},              // 0xa9
    {"xor d", NULL, 1},              // 0xaa
    {"xor e", NULL, 1},              // 0xab
    {"xor h", NULL, 1},              // 0xac
    {"xor l", NULL, 1},              // 0xad
    {"xor (hl)", NULL, 1},           // 0xae
    {"xor a", NULL, 1},              // 0xaf
    {"or b", NULL, 1},               // 0xb0
    {"or c", NULL, 1},               // 0xb1
    {"or d", NULL, 1},               // 0xb2
    {"or e", NULL, 1},               // 0xb3
    {"or h", NULL, 1},               // 0xb4
    {"or l", NULL, 1},               // 0xb5
    {"or (hl)", NULL, 1},            // 0xb6
    {"or a", NULL, 1},               // 0xb7
    {"cp b", NULL, 1},               // 0xb8
    {"cp c", NULL, 1},               // 0xb9
    {"cp d", NULL, 1},               // 0xba
    {"cp e", NULL, 1},               // 0xbb
    {"cp h", NULL, 1},               // 0xbc
    {"cp l", NULL, 1},               // 0xbd
    {"cp (hl)", NULL, 1},            // 0xbe
    {"cp a", NULL, 1},               // 0xbf
    {"ret nz", NULL, 1},             // 0xc0
    {"pop bc", NULL, 1},             // 0xc1
    {"jp nz, $", NULL, 3},           // 0xc2
    {"jp $", NULL, 3},               // 0xc3
    {"call nz, $", NULL, 3},         // 0xc4
    {"push bc", NULL, 1},            // 0xc5
    {"add a, $", NULL, 2},           // 0xc6
    {"rst $00", NULL, 1},            // 0xc7
    {"ret z", NULL, 1},              // 0xc8
    {"ret", NULL, 1},                // 0xc9
    {"jp z, $", NULL, 3},            // 0xca
    {NULL, NULL, 2},                 // 0xcb
    {"call z, $", NULL, 3},          // 0xcc
    {"call $", NULL, 3},             // 0xcd
    {"adc $", NULL, 2},              // 0xce
    {"rst $08", NULL, 1},            // 0xcf
    {"ret nc", NULL, 1},             // 0xd0
    {"pop de", NULL, 1},             // 0xd1
    {"jp nc, $", NULL, 3},           // 0xd2
    {NULL, NULL, 1},                 // 0xd3
    {"call nc, $", NULL, 3},         // 0xd4
    {"push de", NULL, 1},            // 0xd5
    {"sub $", NULL, 2},              // 0xd6
    {"rst $10", NULL, 1},            // 0xd7
    {"ret c", NULL, 1},              // 0xd8
    {"reti", NULL, 1},               // 0xd9
    {"jp c, $", NULL, 3},            // 0xda
    {NULL, NULL, 1},                 // 0xdb
    {"call c, $", NULL, 3},          // 0xdc
    {NULL, NULL, 1},                 // 0xdd
    {"sbc $", NULL, 2},              // 0xde
    {"rst $18", NULL, 1},            // 0xdf
    {"ld ($ff00 + $", "), a", 2},    // 0xe0
    {"pop hl", NULL, 1},             // 0xe1
    {"ld ($ff00 + c), a", NULL, 1},  // 0xe2
    {NULL, NULL, 1},                 // 0xe3
    {NULL, NULL, 1},                 // 0xe4
    {"push hl", NULL, 1},            // 0xe5
    {"and $", NULL, 2},              // 0xe6
    {"rst $20", NULL, 1},            // 0xe7
    {"add sp,$", NULL, 2},           // 0xe8
    {"jp hl", NULL, 1},              // 0xe9
    {"ld ($", "), a", 3},            // 0xea
    {NULL, NULL, 1},                 // 0xeb
    {NULL, NULL, 1},                 // 0xec
    {NULL, NULL, 1},                 // 0xed
    {"xor $", NULL, 2},              // 0xee
    {"rst $28", NULL, 1},            // 0xef
    {"ld a, ($ff00 + $", ")", 2},    // 0xf0
    {"pop af", NULL, 1},             // 0xf1
    {"ld a, ($ff00 + c)", NULL, 1},  // 0xf2
    {"di", NULL, 1},                 // 0xf3
    {NULL, NULL, 1},                 // 0xf4
    {"push af", NULL, 1},            // 0xf5
    {"or $", NULL, 2},               // 0xf6
    {"rst $30", NULL, 1},            // 0xf7
    {"ld hl, sp + $", NULL, 2},      // 0xf8
    {"ld sp, hl", NULL, 1},          // 0xf9
    {"ld a, ($", ")", 3},            // 0xfa
    {"ei", NULL, 1},                 // 0xfb
    {NULL, NULL, 1},                 // 0xfc
    {NULL, NULL, 1},                 // 0xfd
    {"cp $", NULL, 2},               // 0xfe
    {"rst $38", NULL, 1},            // 0xff
};

typedef struct {
    const char *asm1;
} ext_instruction_t;

const ext_instruction_t cb_instr[256] = {
    {"rlc b"},        // 0x00
    {"rlc c"},        // 0x01
    {"rlc d"},        // 0x02
    {"rlc e"},        // 0x03
    {"rlc h"},        // 0x04
    {"rlc l"},        // 0x05
    {"rlc (hl)"},     // 0x06
    {"rlc a"},        // 0x07
    {"rrc b"},        // 0x08
    {"rrc c"},        // 0x09
    {"rrc d"},        // 0x0a
    {"rrc e"},        // 0x0b
    {"rrc h"},        // 0x0c
    {"rrc l"},        // 0x0d
    {"rrc (hl)"},     // 0x0e
    {"rrc a"},        // 0x0f
    {"rl b"},         // 0x10
    {"rl c"},         // 0x11
    {"rl d"},         // 0x12
    {"rl e"},         // 0x13
    {"rl h"},         // 0x14
    {"rl l"},         // 0x15
    {"rl (hl)"},      // 0x16
    {"rl a"},         // 0x17
    {"rr b"},         // 0x18
    {"rr c"},         // 0x19
    {"rr d"},         // 0x1a
    {"rr e"},         // 0x1b
    {"rr h"},         // 0x1c
    {"rr l"},         // 0x1d
    {"rr (hl)"},      // 0x1e
    {"rr a"},         // 0x1f
    {"sla b"},        // 0x20
    {"sla c"},        // 0x21
    {"sla d"},        // 0x22
    {"sla e"},        // 0x23
    {"sla h"},        // 0x24
    {"sla l"},        // 0x25
    {"sla (hl)"},     // 0x26
    {"sla a"},        // 0x27
    {"sra b"},        // 0x28
    {"sra c"},        // 0x29
    {"sra d"},        // 0x2a
    {"sra e"},        // 0x2b
    {"sra h"},        // 0x2c
    {"sra l"},        // 0x2d
    {"sra (hl)"},     // 0x2e
    {"sra a"},        // 0x2f
    {"swap b"},       // 0x30
    {"swap c"},       // 0x31
    {"swap d"},       // 0x32
    {"swap e"},       // 0x33
    {"swap h"},       // 0x34
    {"swap l"},       // 0x35
    {"swap (hl)"},    // 0x36
    {"swap a"},       // 0x37
    {"srl b"},        // 0x38
    {"srl c"},        // 0x39
    {"srl d"},        // 0x3a
    {"srl e"},        // 0x3b
    {"srl h"},        // 0x3c
    {"srl l"},        // 0x3d
    {"srl (hl)"},     // 0x3e
    {"srl a"},        // 0x3f
    {"bit 0, b"},     // 0x40
    {"bit 0, c"},     // 0x41
    {"bit 0, d"},     // 0x42
    {"bit 0, e"},     // 0x43
    {"bit 0, h"},     // 0x44
    {"bit 0, l"},     // 0x45
    {"bit 0, (hl)"},  // 0x46
    {"bit 0, a"},     // 0x47
    {"bit 1, b"},     // 0x48
    {"bit 1, c"},     // 0x49
    {"bit 1, d"},     // 0x4a
    {"bit 1, e"},     // 0x4b
    {"bit 1, h"},     // 0x4c
    {"bit 1, l"},     // 0x4d
    {"bit 1, (hl)"},  // 0x4e
    {"bit 1, a"},     // 0x4f
    {"bit 2, b"},     // 0x50
    {"bit 2, c"},     // 0x51
    {"bit 2, d"},     // 0x52
    {"bit 2, e"},     // 0x53
    {"bit 2, h"},     // 0x54
    {"bit 2, l"},     // 0x55
    {"bit 2, (hl)"},  // 0x56
    {"bit 2, a"},     // 0x57
    {"bit 3, b"},     // 0x58
    {"bit 3, c"},     // 0x59
    {"bit 3, d"},     // 0x5a
    {"bit 3, e"},     // 0x5b
    {"bit 3, h"},     // 0x5c
    {"bit 3, l"},     // 0x5d
    {"bit 3, (hl)"},  // 0x5e
    {"bit 3, a"},     // 0x5f
    {"bit 4, b"},     // 0x60
    {"bit 4, c"},     // 0x61
    {"bit 4, d"},     // 0x62
    {"bit 4, e"},     // 0x63
    {"bit 4, h"},     // 0x64
    {"bit 4, l"},     // 0x65
    {"bit 4, (hl)"},  // 0x66
    {"bit 4, a"},     // 0x67
    {"bit 5, b"},     // 0x68
    {"bit 5, c"},     // 0x69
    {"bit 5, d"},     // 0x6a
    {"bit 5, e"},     // 0x6b
    {"bit 6, h"},     // 0x6c
    {"bit 6, l"},     // 0x6d
    {"bit 5, (hl)"},  // 0x6e
    {"bit 5, a"},     // 0x6f
    {"bit 6, b"},     // 0x70
    {"bit 6, c"},     // 0x71
    {"bit 6, d"},     // 0x72
    {"bit 6, e"},     // 0x73
    {"bit 6, h"},     // 0x74
    {"bit 6, l"},     // 0x75
    {"bit 6, (hl)"},  // 0x76
    {"bit 6, a"},     // 0x77
    {"bit 7, b"},     // 0x78
    {"bit 7, c"},     // 0x79
    {"bit 7, d"},     // 0x7a
    {"bit 7, e"},     // 0x7b
    {"bit 7, h"},     // 0x7c
    {"bit 7, l"},     // 0x7d
    {"bit 7, (hl)"},  // 0x7e
    {"bit 7, a"},     // 0x7f
    {"res 0, b"},     // 0x80
    {"res 0, c"},     // 0x81
    {"res 0, d"},     // 0x82
    {"res 0, e"},     // 0x83
    {"res 0, h"},     // 0x84
    {"res 0, l"},     // 0x85
    {"res 0, (hl)"},  // 0x86
    {"res 0, a"},     // 0x87
    {"res 1, b"},     // 0x88
    {"res 1, c"},     // 0x89
    {"res 1, d"},     // 0x8a
    {"res 1, e"},     // 0x8b
    {"res 1, h"},     // 0x8c
    {"res 1, l"},     // 0x8d
    {"res 1, (hl)"},  // 0x8e
    {"res 1, a"},     // 0x8f
    {"res 2, b"},     // 0x90
    {"res 2, c"},     // 0x91
    {"res 2, d"},     // 0x92
    {"res 2, e"},     // 0x93
    {"res 2, h"},     // 0x94
    {"res 2, l"},     // 0x95
    {"res 2, (hl)"},  // 0x96
    {"res 2, a"},     // 0x97
    {"res 3, b"},     // 0x98
    {"res 3, c"},     // 0x99
    {"res 3, d"},     // 0x9a
    {"res 3, e"},     // 0x9b
    {"res 3, h"},     // 0x9c
    {"res 3, l"},     // 0x9d
    {"res 3, (hl)"},  // 0x9e
    {"res 3, a"},     // 0x9f
    {"res 4, b"},     // 0xa0
    {"res 4, c"},     // 0xa1
    {"res 4, d"},     // 0xa2
    {"res 4, e"},     // 0xa3
    {"res 4, h"},     // 0xa4
    {"res 4, l"},     // 0xa5
    {"res 4, (hl)"},  // 0xa6
    {"res 4, a"},     // 0xa7
    {"res 5, b"},     // 0xa8
    {"res 5, c"},     // 0xa9
    {"res 5, d"},     // 0xaa
    {"res 5, e"},     // 0xab
    {"res 5, h"},     // 0xac
    {"res 5, l"},     // 0xad
    {"res 5, (hl)"},  // 0xae
    {"res 5, a"},     // 0xaf
    {"res 6, b"},     // 0xb0
    {"res 6, c"},     // 0xb1
    {"res 6, d"},     // 0xb2
    {"res 6, e"},     // 0xb3
    {"res 6, h"},     // 0xb4
    {"res 6, l"},     // 0xb5
    {"res 6, (hl)"},  // 0xb6
    {"res 6, a"},     // 0xb7
    {"res 7, b"},     // 0xb8
    {"res 7, c"},     // 0xb9
    {"res 7, d"},     // 0xba
    {"res 7, e"},     // 0xbb
    {"res 7, h"},     // 0xbc
    {"res 7, l"},     // 0xbd
    {"res 7, (hl)"},  // 0xbe
    {"res 7, a"},     // 0xbf
    {"set 0, b"},     // 0xc0
    {"set 0, c"},     // 0xc1
    {"set 0, d"},     // 0xc2
    {"set 0, e"},     // 0xc3
    {"set 0, h"},     // 0xc4
    {"set 0, l"},     // 0xc5
    {"set 0, (hl)"},  // 0xc6
    {"set 0, a"},     // 0xc7
    {"set 1, b"},     // 0xc8
    {"set 1, c"},     // 0xc9
    {"set 1, d"},     // 0xca
    {"set 1, e"},     // 0xcb
    {"set 1, h"},     // 0xcc
    {"set 1, l"},     // 0xcd
    {"set 1, (hl)"},  // 0xce
    {"set 1, a"},     // 0xcf
    {"set 2, b"},     // 0xd0
    {"set 2, c"},     // 0xd1
    {"set 2, d"},     // 0xd2
    {"set 2, e"},     // 0xd3
    {"set 2, h"},     // 0xd4
    {"set 2, l"},     // 0xd5
    {"set 2, (hl)"},  // 0xd6
    {"set 2, a"},     // 0xd7
    {"set 3, b"},     // 0xd8
    {"set 3, c"},     // 0xd9
    {"set 3, d"},     // 0xda
    {"set 3, e"},     // 0xdb
    {"set 3, h"},     // 0xdc
    {"set 3, l"},     // 0xdd
    {"set 3, (hl)"},  // 0xde
    {"set 3, a"},     // 0xdf
    {"set 4, b"},     // 0xe0
    {"set 4, c"},     // 0xe1
    {"set 4, d"},     // 0xe2
    {"set 4, e"},     // 0xe3
    {"set 4, h"},     // 0xe4
    {"set 4, l"},     // 0xe5
    {"set 4, (hl)"},  // 0xe6
    {"set 4, a"},     // 0xe7
    {"set 5, b"},     // 0xe8
    {"set 5, c"},     // 0xe9
    {"set 5, d"},     // 0xea
    {"set 5, e"},     // 0xeb
    {"set 5, h"},     // 0xec
    {"set 5, l"},     // 0xed
    {"set 5, (hl)"},  // 0xee
    {"set 5, a"},     // 0xef
    {"set 6, b"},     // 0xf0
    {"set 6, c"},     // 0xf1
    {"set 6, d"},     // 0xf2
    {"set 6, e"},     // 0xf3
    {"set 6, h"},     // 0xf4
    {"set 6, l"},     // 0xf5
    {"set 6, (hl)"},  // 0xf6
    {"set 6, a"},     // 0xf7
    {"set 7, b"},     // 0xf8
    {"set 7, c"},     // 0xf9
    {"set 7, d"},     // 0xfa
    {"set 7, e"},     // 0xfb
    {"set 7, h"},     // 0xfc
    {"set 7, l"},     // 0xfd
    {"set 7, (hl)"},  // 0xfe
    {"set 7, a"},     // 0xff
};

typedef struct {
    uint8_t *rom;
    size_t rom_size;
    FILE *output;
} objdump_t;

static objdump_t *objdump_init(char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }
    objdump_t *obj = malloc(sizeof(objdump_t));
    /* Get file size. */
    fseek(file, 0, SEEK_END);
    obj->rom_size = (size_t)ftell(file);
    rewind(file);
    /* Init ROM. */
    obj->rom = malloc(obj->rom_size);
    /* Read rom to memory. */
    size_t read_size = fread(obj->rom, 1, obj->rom_size, file);
    fclose(file);
    if (read_size != obj->rom_size) {
        fprintf(stderr, "ERROR: fread\n");
        return NULL;
    }
    return obj;
}

static void objdump_finish(objdump_t *obj)
{
    free(obj->rom);
    free(obj);
}

static void objdump_data(objdump_t *obj, uint8_t data)
{
    fprintf(obj->output, ".data $%02x\n", data);
}

static void objdump_cb(objdump_t *obj, size_t pc)
{
    uint8_t opcode = obj->rom[pc++];
    fprintf(obj->output, "%s\n", cb_instr[opcode].asm1);
}

static void objdump_instruction(objdump_t *obj, size_t pc)
{
    uint8_t opcode = obj->rom[pc++];
    uint8_t instr_length = instr[opcode].length;
    const char *asm1 = instr[opcode].asm1;
    const char *asm2 = instr[opcode].asm2 ? instr[opcode].asm2 : "";
    if (asm1 == NULL) {
        if (instr_length == 1) {
            objdump_data(obj, opcode);
        } else {
            objdump_cb(obj, pc);
        }
        return;
    }
    if (instr_length == 1) {
        fprintf(obj->output, "%s\n", asm1);
    } else if (instr_length == 2) {
        uint8_t operand = obj->rom[pc];
        fprintf(obj->output, "%s%02x%s\n", asm1, operand, asm2);
    } else if (instr_length == 3) {
        uint16_t operand = (uint16_t)(obj->rom[pc] | (obj->rom[pc + 1] << 8));
        fprintf(obj->output, "%s%04x%s\n", asm1, operand, asm2);
    } else {
        fprintf(stderr, "ERROR: invalid operand length!\n");
        exit(EXIT_FAILURE);
    }
}

static void objdump_dump(objdump_t *obj)
{
    obj->output = fopen("out.s", "w");
    size_t pc = 0;
    while (pc < obj->rom_size) {
        while (pc >= 0x0104 && pc <= 0x014f) {
            objdump_data(obj, obj->rom[pc++]);
        }
        objdump_instruction(obj, pc);
        uint8_t opcode = obj->rom[pc];
        pc += instr[opcode].length;
    }
    fclose(obj->output);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "ERROR: syntax: %s <rom file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    objdump_t *obj = objdump_init(argv[1]);
    if (obj == NULL) {
        fprintf(stderr, "ERROR: could not load rom %s\n", argv[1]);
        objdump_finish(obj);
        exit(EXIT_FAILURE);
    }
    objdump_dump(obj);
    objdump_finish(obj);
    return 0;
}
