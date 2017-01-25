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
    {"ldi (hl), a", NULL, 1},        // 0x22
    {"inc hl", NULL, 1},             // 0x23
    {"inc h", NULL, 1},              // 0x24
    {"dec h", NULL, 1},              // 0x25
    {"ld h, $", NULL, 2},            // 0x26
    {"daa", NULL, 1},                // 0x27
    {"jr z, $", NULL, 2},            // 0x28
    {"add hl, hl", NULL, 1},         // 0x29
    {"ldi a, (hl)", NULL, 1},        // 0x2a
    {"dec hl", NULL, 1},             // 0x2b
    {"inc l", NULL, 1},              // 0x2c
    {"dec l", NULL, 1},              // 0x2d
    {"ld l, $", NULL, 2},            // 0x2e
    {"cpl", NULL, 1},                // 0x2f
    {"jr nc, $", NULL, 2},           // 0x30
    {"ld sp, $", NULL, 3},           // 0x31
    {"ldd (hl), a", NULL, 1},        // 0x32
    {"inc sp", NULL, 1},             // 0x33
    {"inc (hl)", NULL, 1},           // 0x34
    {"dec (hl)", NULL, 1},           // 0x35
    {"ld (hl), $", NULL, 2},         // 0x36
    {"scf", NULL, 1},                // 0x37
    {"jr c, $", NULL, 2},            // 0x38
    {"add hl, sp", NULL, 1},         // 0x39
    {"ldd a, (hl)", NULL, 1},        // 0x3a
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
    {"add a", NULL, 1},              // 0x87
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
    {"cb ", NULL, 2},                // 0xcb
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
    {"ldh ($", "), a", 2},    // 0xe0
    {"pop hl", NULL, 1},             // 0xe1
    {"ld (c), a", NULL, 1},  // 0xe2
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
    {"ldh a, ($", ")", 2},    // 0xf0
    {"pop af", NULL, 1},             // 0xf1
    {"ld a, ($ff00 + c)", NULL, 1},  // 0xf2
    {"di", NULL, 1},                 // 0xf3
    {NULL, NULL, 1},                 // 0xf4
    {"push af", NULL, 1},            // 0xf5
    {"or $", NULL, 2},               // 0xf6
    {"rst $30", NULL, 1},            // 0xf7
    {"ld hl, sp+$", NULL, 2},        // 0xf8
    {"ld sp, hl", NULL, 1},          // 0xf9
    {"ld a, ($", ")", 3},            // 0xfa
    {"ei", NULL, 1},                 // 0xfb
    {NULL, NULL, 1},                 // 0xfc
    {NULL, NULL, 1},                 // 0xfd
    {"cp $", NULL, 2},               // 0xfe
    {"rst $38", NULL, 1},            // 0xff
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
    fprintf(obj->output, "db $%02x\n", data);
}

static void objdump_instruction(objdump_t *obj, size_t pc)
{
    uint8_t opcode = obj->rom[pc++];
    uint8_t instr_length = instr[opcode].length;
    const char *asm1 = instr[opcode].asm1;
    const char *asm2 = instr[opcode].asm2 ? instr[opcode].asm2 : "";
    if (asm1 == NULL) {
        objdump_data(obj, opcode);
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
    obj->output = fopen("out.as", "w");
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
