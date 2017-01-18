#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    const char *asm1;
    const char *asm2;
    uint8_t length;
} instruction_t;

const instruction_t instr[256] = {
    {"NOP", NULL, 1},                // 0x00
    {"LD BC, $", NULL, 3},           // 0x01
    {"LD (BC), A", NULL, 1},         // 0x02
    {"INC BC", NULL, 1},             // 0x03
    {"INC B", NULL, 1},              // 0x04
    {"DEC B", NULL, 1},              // 0x05
    {"LD B, $", NULL, 2},            // 0x06
    {"RLCA", NULL, 1},               // 0x07
    {"LD ($", "), SP", 3},           // 0x08
    {"ADD HL, BC", NULL, 1},         // 0x09
    {"LD A, (BC)", NULL, 1},         // 0x0a
    {"DEC BC", NULL, 1},             // 0x0b
    {"INC C", NULL, 1},              // 0x0c
    {"DEC C", NULL, 1},              // 0x0d
    {"LD C, $", NULL, 2},            // 0x0e
    {"RRCA", NULL, 1},               // 0x0f
    {"STOP", NULL, 1},               // 0x10
    {"LD DE, $", NULL, 3},           // 0x11
    {"LD (DE), A", NULL, 1},         // 0x12
    {"INC DE", NULL, 1},             // 0x13
    {"INC D", NULL, 1},              // 0x14
    {"DEC D", NULL, 1},              // 0x15
    {"LD D, $", NULL, 2},            // 0x16
    {"RLA", NULL, 1},                // 0x17
    {"JR $", NULL, 2},               // 0x18
    {"ADD HL, DE", NULL, 1},         // 0x19
    {"LD A, (DE)", NULL, 1},         // 0x1a
    {"DEC DE", NULL, 1},             // 0x1b
    {"INC E", NULL, 1},              // 0x1c
    {"DEC E", NULL, 1},              // 0x1d
    {"LD E, $", NULL, 2},            // 0x1e
    {"RRA", NULL, 1},                // 0x1f
    {"JR NZ, $", NULL, 2},           // 0x20
    {"LD HL, $", NULL, 3},           // 0x21
    {"LDI (HL), A", NULL, 1},        // 0x22
    {"INC HL", NULL, 1},             // 0x23
    {"INC H", NULL, 1},              // 0x24
    {"DEC H", NULL, 1},              // 0x25
    {"LD H, $", NULL, 2},            // 0x26
    {"DAA", NULL, 1},                // 0x27
    {"JR Z, $", NULL, 2},            // 0x28
    {"ADD HL, HL", NULL, 1},         // 0x29
    {"LDI A, (HL)", NULL, 1},        // 0x2a
    {"DEC HL", NULL, 1},             // 0x2b
    {"INC L", NULL, 1},              // 0x2c
    {"DEC L", NULL, 1},              // 0x2d
    {"LD L, $", NULL, 2},            // 0x2e
    {"CPL", NULL, 1},                // 0x2f
    {"JR NC, $", NULL, 2},           // 0x30
    {"LD SP, $", NULL, 3},           // 0x31
    {"LDD (HL), A", NULL, 1},        // 0x32
    {"INC SP", NULL, 1},             // 0x33
    {"INC (HL)", NULL, 1},           // 0x34
    {"DEC (HL)", NULL, 1},           // 0x35
    {"LD (HL), $", NULL, 2},         // 0x36
    {"SCF", NULL, 1},                // 0x37
    {"JR C, $", NULL, 2},            // 0x38
    {"ADD HL, SP", NULL, 1},         // 0x39
    {"LDD A, (HL)", NULL, 1},        // 0x3a
    {"DEC SP", NULL, 1},             // 0x3b
    {"INC A", NULL, 1},              // 0x3c
    {"DEC A", NULL, 1},              // 0x3d
    {"LD A, $", NULL, 2},            // 0x3e
    {"CCF", NULL, 1},                // 0x3f
    {"LD B, B", NULL, 1},            // 0x40
    {"LD B, C", NULL, 1},            // 0x41
    {"LD B, D", NULL, 1},            // 0x42
    {"LD B, E", NULL, 1},            // 0x43
    {"LD B, H", NULL, 1},            // 0x44
    {"LD B, L", NULL, 1},            // 0x45
    {"LD B, (HL)", NULL, 1},         // 0x46
    {"LD B, A", NULL, 1},            // 0x47
    {"LD C, B", NULL, 1},            // 0x48
    {"LD C, C", NULL, 1},            // 0x49
    {"LD C, D", NULL, 1},            // 0x4a
    {"LD C, E", NULL, 1},            // 0x4b
    {"LD C, H", NULL, 1},            // 0x4c
    {"LD C, L", NULL, 1},            // 0x4d
    {"LD C, (HL)", NULL, 1},         // 0x4e
    {"LD C, A", NULL, 1},            // 0x4f
    {"LD D, B", NULL, 1},            // 0x50
    {"LD D, C", NULL, 1},            // 0x51
    {"LD D, D", NULL, 1},            // 0x52
    {"LD D, E", NULL, 1},            // 0x53
    {"LD D, H", NULL, 1},            // 0x54
    {"LD D, L", NULL, 1},            // 0x55
    {"LD D, (HL)", NULL, 1},         // 0x56
    {"LD D, A", NULL, 1},            // 0x57
    {"LD E, B", NULL, 1},            // 0x58
    {"LD E, C", NULL, 1},            // 0x59
    {"LD E, D", NULL, 1},            // 0x5a
    {"LD E, E", NULL, 1},            // 0x5b
    {"LD E, H", NULL, 1},            // 0x5c
    {"LD E, L", NULL, 1},            // 0x5d
    {"LD E, (HL)", NULL, 1},         // 0x5e
    {"LD E, A", NULL, 1},            // 0x5f
    {"LD H, B", NULL, 1},            // 0x60
    {"LD H, C", NULL, 1},            // 0x61
    {"LD H, D", NULL, 1},            // 0x62
    {"LD H, E", NULL, 1},            // 0x63
    {"LD H, H", NULL, 1},            // 0x64
    {"LD H, L", NULL, 1},            // 0x65
    {"LD H, (HL)", NULL, 1},         // 0x66
    {"LD H, A", NULL, 1},            // 0x67
    {"LD L, B", NULL, 1},            // 0x68
    {"LD L, C", NULL, 1},            // 0x69
    {"LD L, D", NULL, 1},            // 0x6a
    {"LD L, E", NULL, 1},            // 0x6b
    {"LD L, H", NULL, 1},            // 0x6c
    {"LD L, L", NULL, 1},            // 0x6d
    {"LD L, (HL)", NULL, 1},         // 0x6e
    {"LD L, A", NULL, 1},            // 0x6f
    {"LD (HL), B", NULL, 1},         // 0x70
    {"LD (HL), C", NULL, 1},         // 0x71
    {"LD (HL), D", NULL, 1},         // 0x72
    {"LD (HL), E", NULL, 1},         // 0x73
    {"LD (HL), H", NULL, 1},         // 0x74
    {"LD (HL), L", NULL, 1},         // 0x75
    {"HALT", NULL, 1},               // 0x76
    {"LD (HL), A", NULL, 1},         // 0x77
    {"LD A, B", NULL, 1},            // 0x78
    {"LD A, C", NULL, 1},            // 0x79
    {"LD A, D", NULL, 1},            // 0x7a
    {"LD A, E", NULL, 1},            // 0x7b
    {"LD A, H", NULL, 1},            // 0x7c
    {"LD A, L", NULL, 1},            // 0x7d
    {"LD A, (HL)", NULL, 1},         // 0x7e
    {"LD A, A", NULL, 1},            // 0x7f
    {"ADD A, B", NULL, 1},           // 0x80
    {"ADD A, C", NULL, 1},           // 0x81
    {"ADD A, D", NULL, 1},           // 0x82
    {"ADD A, E", NULL, 1},           // 0x83
    {"ADD A, H", NULL, 1},           // 0x84
    {"ADD A, L", NULL, 1},           // 0x85
    {"ADD A, (HL)", NULL, 1},        // 0x86
    {"ADD A", NULL, 1},              // 0x87
    {"ADC B", NULL, 1},              // 0x88
    {"ADC C", NULL, 1},              // 0x89
    {"ADC D", NULL, 1},              // 0x8a
    {"ADC E", NULL, 1},              // 0x8b
    {"ADC H", NULL, 1},              // 0x8c
    {"ADC L", NULL, 1},              // 0x8d
    {"ADC (HL)", NULL, 1},           // 0x8e
    {"ADC A", NULL, 1},              // 0x8f
    {"SUB B", NULL, 1},              // 0x90
    {"SUB C", NULL, 1},              // 0x91
    {"SUB D", NULL, 1},              // 0x92
    {"SUB E", NULL, 1},              // 0x93
    {"SUB H", NULL, 1},              // 0x94
    {"SUB L", NULL, 1},              // 0x95
    {"SUB (HL)", NULL, 1},           // 0x96
    {"SUB A", NULL, 1},              // 0x97
    {"SBC B", NULL, 1},              // 0x98
    {"SBC C", NULL, 1},              // 0x99
    {"SBC D", NULL, 1},              // 0x9a
    {"SBC E", NULL, 1},              // 0x9b
    {"SBC H", NULL, 1},              // 0x9c
    {"SBC L", NULL, 1},              // 0x9d
    {"SBC (HL)", NULL, 1},           // 0x9e
    {"SBC A", NULL, 1},              // 0x9f
    {"AND B", NULL, 1},              // 0xa0
    {"AND C", NULL, 1},              // 0xa1
    {"AND D", NULL, 1},              // 0xa2
    {"AND E", NULL, 1},              // 0xa3
    {"AND H", NULL, 1},              // 0xa4
    {"AND L", NULL, 1},              // 0xa5
    {"AND (HL)", NULL, 1},           // 0xa6
    {"AND A", NULL, 1},              // 0xa7
    {"XOR B", NULL, 1},              // 0xa8
    {"XOR C", NULL, 1},              // 0xa9
    {"XOR D", NULL, 1},              // 0xaa
    {"XOR E", NULL, 1},              // 0xab
    {"XOR H", NULL, 1},              // 0xac
    {"XOR L", NULL, 1},              // 0xad
    {"XOR (HL)", NULL, 1},           // 0xae
    {"XOR A", NULL, 1},              // 0xaf
    {"OR B", NULL, 1},               // 0xb0
    {"OR C", NULL, 1},               // 0xb1
    {"OR D", NULL, 1},               // 0xb2
    {"OR E", NULL, 1},               // 0xb3
    {"OR H", NULL, 1},               // 0xb4
    {"OR L", NULL, 1},               // 0xb5
    {"OR (HL)", NULL, 1},            // 0xb6
    {"OR A", NULL, 1},               // 0xb7
    {"CP B", NULL, 1},               // 0xb8
    {"CP C", NULL, 1},               // 0xb9
    {"CP D", NULL, 1},               // 0xba
    {"CP E", NULL, 1},               // 0xbb
    {"CP H", NULL, 1},               // 0xbc
    {"CP L", NULL, 1},               // 0xbd
    {"CP (HL)", NULL, 1},            // 0xbe
    {"CP A", NULL, 1},               // 0xbf
    {"RET NZ", NULL, 1},             // 0xc0
    {"POP BC", NULL, 1},             // 0xc1
    {"JP NZ, $", NULL, 3},           // 0xc2
    {"JP $", NULL, 3},               // 0xc3
    {"CALL NZ, $", NULL, 3},         // 0xc4
    {"PUSH BC", NULL, 1},            // 0xc5
    {"ADD A, $", NULL, 2},           // 0xc6
    {"RST $00", NULL, 1},            // 0xc7
    {"RET Z", NULL, 1},              // 0xc8
    {"RET", NULL, 1},                // 0xc9
    {"JP Z, $", NULL, 3},            // 0xca
    {"CB ", NULL, 2},                // 0xcb
    {"CALL Z, $", NULL, 3},          // 0xcc
    {"CALL $", NULL, 3},             // 0xcd
    {"ADC $", NULL, 2},              // 0xce
    {"RST $08", NULL, 1},            // 0xcf
    {"RET NC", NULL, 1},             // 0xd0
    {"POP DE", NULL, 1},             // 0xd1
    {"JP NC, $", NULL, 3},           // 0xd2
    {"UNKNOWN", NULL, 1},            // 0xd3
    {"CALL NC, $", NULL, 3},         // 0xd4
    {"PUSH DE", NULL, 1},            // 0xd5
    {"SUB $", NULL, 2},              // 0xd6
    {"RST $10", NULL, 1},            // 0xd7
    {"RET C", NULL, 1},              // 0xd8
    {"RETI", NULL, 1},               // 0xd9
    {"JP C, $", NULL, 3},            // 0xda
    {"UNKNOWN", NULL, 1},            // 0xdb
    {"CALL C, $", NULL, 3},          // 0xdc
    {"UNKNOWN", NULL, 1},            // 0xdd
    {"SBC $", NULL, 2},              // 0xde
    {"RST $18", NULL, 1},            // 0xdf
    {"LD ($FF00 + $", "), A", 2},    // 0xe0
    {"POP HL", NULL, 1},             // 0xe1
    {"LD ($FF00 + C), A", NULL, 1},  // 0xe2
    {"UNKNOWN", NULL, 1},            // 0xe3
    {"UNKNOWN", NULL, 1},            // 0xe4
    {"PUSH HL", NULL, 1},            // 0xe5
    {"AND $", NULL, 2},              // 0xe6
    {"RST $20", NULL, 1},            // 0xe7
    {"ADD SP,$", NULL, 2},           // 0xe8
    {"JP HL", NULL, 1},              // 0xe9
    {"LD ($", "), A", 3},            // 0xea
    {"UNKNOWN", NULL, 1},            // 0xeb
    {"UNKNOWN", NULL, 1},            // 0xec
    {"UNKNOWN", NULL, 1},            // 0xed
    {"XOR $", NULL, 2},              // 0xee
    {"RST $28", NULL, 1},            // 0xef
    {"LD A, ($FF00 + $", ")", 2},    // 0xf0
    {"POP AF", NULL, 1},             // 0xf1
    {"LD A, ($FF00 + C)", NULL, 1},  // 0xf2
    {"DI", NULL, 1},                 // 0xf3
    {"UNKNOWN", NULL, 1},            // 0xf4
    {"PUSH AF", NULL, 1},            // 0xf5
    {"OR $", NULL, 2},               // 0xf6
    {"RST $30", NULL, 1},            // 0xf7
    {"LD HL, SP+$", NULL, 2},        // 0xf8
    {"LD SP, HL", NULL, 1},          // 0xf9
    {"LD A, ($", ")", 3},            // 0xfa
    {"EI", NULL, 1},                 // 0xfb
    {"UNKNOWN", NULL, 1},            // 0xfc
    {"UNKNOWN", NULL, 1},            // 0xfd
    {"CP $", NULL, 2},               // 0xfe
    {"RST $38", NULL, 1},            // 0xff
};

typedef struct {
    uint8_t *rom;
    size_t rom_size;
    FILE *output;
} gusdump_t;

static gusdump_t *gusdump_init(char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }
    gusdump_t *obj = malloc(sizeof(gusdump_t));
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

static void gusdump_finish(gusdump_t *obj)
{
    free(obj->rom);
    free(obj);
}

static void gusdump_instruction(gusdump_t *obj, size_t pc)
{
    uint8_t opcode = obj->rom[pc++];
    uint8_t instr_length = instr[opcode].length;
    const char *asm1 = instr[opcode].asm1;
    const char *asm2 = instr[opcode].asm2 ? instr[opcode].asm2 : "";
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

static void gusdump_dump(gusdump_t *obj)
{
    obj->output = fopen("out.as", "w");
    size_t pc = 0;
    while (pc < obj->rom_size) {
        gusdump_instruction(obj, pc);
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
    gusdump_t *obj = gusdump_init(argv[1]);
    if (obj == NULL) {
        fprintf(stderr, "ERROR: could not load rom %s\n", argv[1]);
        gusdump_finish(obj);
        exit(EXIT_FAILURE);
    }
    gusdump_dump(obj);
    gusdump_finish(obj);
    return 0;
}
