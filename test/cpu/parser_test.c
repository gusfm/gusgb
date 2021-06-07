#include "as/parser.h"
#include "asm.h"
#include "ut.h"

void parser_test(void);

static int adc_test(void)
{
    ASSERT(asm_test("adc a,b", 0x88) == PARSER_OK);
    ASSERT(asm_test("adc a,c", 0x89) == PARSER_OK);
    ASSERT(asm_test("adc a,d", 0x8a) == PARSER_OK);
    ASSERT(asm_test("adc a,e", 0x8b) == PARSER_OK);
    ASSERT(asm_test("adc a,h", 0x8c) == PARSER_OK);
    ASSERT(asm_test("adc a,l", 0x8d) == PARSER_OK);
    ASSERT(asm_test("adc a,(hl)", 0x8e) == PARSER_OK);
    ASSERT(asm_test("adc a,a", 0x8f) == PARSER_OK);
    ASSERT(asm_test("adc a,0", 0xce) == PARSER_OK);
    ASSERT(asm_test("adc a,16", 0x10ce) == PARSER_OK);
    ASSERT(asm_test("adc a,0xff", 0xffce) == PARSER_OK);
    ASSERT(asm_test("adc a,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("adc a,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("adc a,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("adc a,(bc)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("adc b,a", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int add_test(void)
{
    ASSERT(asm_test("add a,b", 0x80) == PARSER_OK);
    ASSERT(asm_test("add a,c", 0x81) == PARSER_OK);
    ASSERT(asm_test("add a,d", 0x82) == PARSER_OK);
    ASSERT(asm_test("add a,e", 0x83) == PARSER_OK);
    ASSERT(asm_test("add a,h", 0x84) == PARSER_OK);
    ASSERT(asm_test("add a,l", 0x85) == PARSER_OK);
    ASSERT(asm_test("add a,(hl)", 0x86) == PARSER_OK);
    ASSERT(asm_test("add a,a", 0x87) == PARSER_OK);
    ASSERT(asm_test("add a,0", 0xc6) == PARSER_OK);
    ASSERT(asm_test("add a,16", 0x10c6) == PARSER_OK);
    ASSERT(asm_test("add a,0xff", 0xffc6) == PARSER_OK);
    ASSERT(asm_test("add a,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("add a,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("add a,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("add a,(bc)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("add b,a", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("add hl,bc", 0x09) == PARSER_OK);
    ASSERT(asm_test("add hl,de", 0x19) == PARSER_OK);
    ASSERT(asm_test("add hl,hl", 0x29) == PARSER_OK);
    ASSERT(asm_test("add hl,sp", 0x39) == PARSER_OK);
    ASSERT(asm_test("add hl,af", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("add hl,10", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("add sp,16", 0x10e8) == PARSER_OK);
    ASSERT(asm_test("add sp,0xff", 0xffe8) == PARSER_OK);
    ASSERT(asm_test("add sp,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("add sp,256", 0) == PARSER_ERR_NUMBER);
    return 0;
}

static int and_test(void)
{
    ASSERT(asm_test("and b", 0xa0) == PARSER_OK);
    ASSERT(asm_test("and c", 0xa1) == PARSER_OK);
    ASSERT(asm_test("and d", 0xa2) == PARSER_OK);
    ASSERT(asm_test("and e", 0xa3) == PARSER_OK);
    ASSERT(asm_test("and h", 0xa4) == PARSER_OK);
    ASSERT(asm_test("and l", 0xa5) == PARSER_OK);
    ASSERT(asm_test("and (hl)", 0xa6) == PARSER_OK);
    ASSERT(asm_test("and a", 0xa7) == PARSER_OK);
    ASSERT(asm_test("and 0", 0xe6) == PARSER_OK);
    ASSERT(asm_test("and 16", 0x10e6) == PARSER_OK);
    ASSERT(asm_test("and 0xff", 0xffe6) == PARSER_OK);
    ASSERT(asm_test("and 0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("and 256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("and x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("and (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int bit_test(void)
{
    ASSERT(asm_test("bit 0,b", 0x40cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,c", 0x41cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,d", 0x42cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,e", 0x43cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,h", 0x44cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,l", 0x45cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,(hl)", 0x46cb) == PARSER_OK);
    ASSERT(asm_test("bit 0,a", 0x47cb) == PARSER_OK);
    ASSERT(asm_test("bit 1,b", 0x48cb) == PARSER_OK);
    ASSERT(asm_test("bit 1,c", 0x49cb) == PARSER_OK);
    ASSERT(asm_test("bit 1,d", 0x4acb) == PARSER_OK);
    ASSERT(asm_test("bit 1,e", 0x4bcb) == PARSER_OK);
    ASSERT(asm_test("bit 1,h", 0x4ccb) == PARSER_OK);
    ASSERT(asm_test("bit 1,l", 0x4dcb) == PARSER_OK);
    ASSERT(asm_test("bit 1,(hl)", 0x4ecb) == PARSER_OK);
    ASSERT(asm_test("bit 1,a", 0x4fcb) == PARSER_OK);
    ASSERT(asm_test("bit 2,b", 0x50cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,c", 0x51cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,d", 0x52cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,e", 0x53cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,h", 0x54cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,l", 0x55cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,(hl)", 0x56cb) == PARSER_OK);
    ASSERT(asm_test("bit 2,a", 0x57cb) == PARSER_OK);
    ASSERT(asm_test("bit 3,b", 0x58cb) == PARSER_OK);
    ASSERT(asm_test("bit 3,c", 0x59cb) == PARSER_OK);
    ASSERT(asm_test("bit 3,d", 0x5acb) == PARSER_OK);
    ASSERT(asm_test("bit 3,e", 0x5bcb) == PARSER_OK);
    ASSERT(asm_test("bit 3,h", 0x5ccb) == PARSER_OK);
    ASSERT(asm_test("bit 3,l", 0x5dcb) == PARSER_OK);
    ASSERT(asm_test("bit 3,(hl)", 0x5ecb) == PARSER_OK);
    ASSERT(asm_test("bit 3,a", 0x5fcb) == PARSER_OK);
    ASSERT(asm_test("bit 4,b", 0x60cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,c", 0x61cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,d", 0x62cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,e", 0x63cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,h", 0x64cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,l", 0x65cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,(hl)", 0x66cb) == PARSER_OK);
    ASSERT(asm_test("bit 4,a", 0x67cb) == PARSER_OK);
    ASSERT(asm_test("bit 5,b", 0x68cb) == PARSER_OK);
    ASSERT(asm_test("bit 5,c", 0x69cb) == PARSER_OK);
    ASSERT(asm_test("bit 5,d", 0x6acb) == PARSER_OK);
    ASSERT(asm_test("bit 5,e", 0x6bcb) == PARSER_OK);
    ASSERT(asm_test("bit 5,h", 0x6ccb) == PARSER_OK);
    ASSERT(asm_test("bit 5,l", 0x6dcb) == PARSER_OK);
    ASSERT(asm_test("bit 5,(hl)", 0x6ecb) == PARSER_OK);
    ASSERT(asm_test("bit 5,a", 0x6fcb) == PARSER_OK);
    ASSERT(asm_test("bit 6,b", 0x70cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,c", 0x71cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,d", 0x72cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,e", 0x73cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,h", 0x74cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,l", 0x75cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,(hl)", 0x76cb) == PARSER_OK);
    ASSERT(asm_test("bit 6,a", 0x77cb) == PARSER_OK);
    ASSERT(asm_test("bit 7,b", 0x78cb) == PARSER_OK);
    ASSERT(asm_test("bit 7,c", 0x79cb) == PARSER_OK);
    ASSERT(asm_test("bit 7,d", 0x7acb) == PARSER_OK);
    ASSERT(asm_test("bit 7,e", 0x7bcb) == PARSER_OK);
    ASSERT(asm_test("bit 7,h", 0x7ccb) == PARSER_OK);
    ASSERT(asm_test("bit 7,l", 0x7dcb) == PARSER_OK);
    ASSERT(asm_test("bit 7,(hl)", 0x7ecb) == PARSER_OK);
    ASSERT(asm_test("bit 7,a", 0x7fcb) == PARSER_OK);
    ASSERT(asm_test("bit 8,b", 0) == PARSER_ERR_NUMBER);
    return 0;
}

static int call_test(void)
{
    ASSERT(asm_test("call nz,256", 0x100c4) == PARSER_OK);
    ASSERT(asm_test("call nz,0xffff", 0xffffc4) == PARSER_OK);
    ASSERT(asm_test("call nz,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("call z,256", 0x100cc) == PARSER_OK);
    ASSERT(asm_test("call z,0xffff", 0xffffcc) == PARSER_OK);
    ASSERT(asm_test("call z,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("call 256", 0x100cd) == PARSER_OK);
    ASSERT(asm_test("call 0xffff", 0xffffcd) == PARSER_OK);
    ASSERT(asm_test("call 0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("call nc,256", 0x100d4) == PARSER_OK);
    ASSERT(asm_test("call nc,0xffff", 0xffffd4) == PARSER_OK);
    ASSERT(asm_test("call nc,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("call c,256", 0x100dc) == PARSER_OK);
    ASSERT(asm_test("call c,0xffff", 0xffffdc) == PARSER_OK);
    ASSERT(asm_test("call c,0x10000", 0) == PARSER_ERR_NUMBER);
    return 0;
}

static int cp_test(void)
{
    ASSERT(asm_test("cp b", 0xb8) == PARSER_OK);
    ASSERT(asm_test("cp c", 0xb9) == PARSER_OK);
    ASSERT(asm_test("cp d", 0xba) == PARSER_OK);
    ASSERT(asm_test("cp e", 0xbb) == PARSER_OK);
    ASSERT(asm_test("cp h", 0xbc) == PARSER_OK);
    ASSERT(asm_test("cp l", 0xbd) == PARSER_OK);
    ASSERT(asm_test("cp (hl)", 0xbe) == PARSER_OK);
    ASSERT(asm_test("cp a", 0xbf) == PARSER_OK);
    ASSERT(asm_test("cp 0", 0xfe) == PARSER_OK);
    ASSERT(asm_test("cp 16", 0x10fe) == PARSER_OK);
    ASSERT(asm_test("cp 0xff", 0xfffe) == PARSER_OK);
    ASSERT(asm_test("cp 0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("cp 256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("cp x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("cp (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int db_test(void)
{
    ASSERT(asm_test("db 0x00", 0x00) == PARSER_OK);
    ASSERT(asm_test("db 0x01", 0x01) == PARSER_OK);
    ASSERT(asm_test("db 0xff", 0xff) == PARSER_OK);
    ASSERT(asm_test("db 0x01, 0x02, 0x03, 0x04", 0x04030201) == PARSER_OK);
    ASSERT(asm_test("db 0x01,", 0x01) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("db \"test string 1234 !@#$%^&*()_+\"", 0x74736574) ==
           PARSER_OK);
    ASSERT(asm_test("db 0x01, \"test\"", 0x73657401) == PARSER_OK);
    ASSERT(asm_test("db \"tes\", 0x01", 0x01736574) == PARSER_OK);
    return 0;
}

static int dec_test(void)
{
    ASSERT(asm_test("dec b", 0x05) == PARSER_OK);
    ASSERT(asm_test("dec bc", 0x0b) == PARSER_OK);
    ASSERT(asm_test("dec c", 0x0d) == PARSER_OK);
    ASSERT(asm_test("dec d", 0x15) == PARSER_OK);
    ASSERT(asm_test("dec de", 0x1b) == PARSER_OK);
    ASSERT(asm_test("dec e", 0x1d) == PARSER_OK);
    ASSERT(asm_test("dec h", 0x25) == PARSER_OK);
    ASSERT(asm_test("dec hl", 0x2b) == PARSER_OK);
    ASSERT(asm_test("dec l", 0x2d) == PARSER_OK);
    ASSERT(asm_test("dec (hl)", 0x35) == PARSER_OK);
    ASSERT(asm_test("dec sp", 0x3b) == PARSER_OK);
    ASSERT(asm_test("dec a", 0x3d) == PARSER_OK);
    ASSERT(asm_test("dec (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int inc_test(void)
{
    ASSERT(asm_test("inc bc", 0x03) == PARSER_OK);
    ASSERT(asm_test("inc b", 0x04) == PARSER_OK);
    ASSERT(asm_test("inc c", 0x0c) == PARSER_OK);
    ASSERT(asm_test("inc de", 0x13) == PARSER_OK);
    ASSERT(asm_test("inc d", 0x14) == PARSER_OK);
    ASSERT(asm_test("inc e", 0x1c) == PARSER_OK);
    ASSERT(asm_test("inc hl", 0x23) == PARSER_OK);
    ASSERT(asm_test("inc h", 0x24) == PARSER_OK);
    ASSERT(asm_test("inc l", 0x2c) == PARSER_OK);
    ASSERT(asm_test("inc sp", 0x33) == PARSER_OK);
    ASSERT(asm_test("inc (hl)", 0x34) == PARSER_OK);
    ASSERT(asm_test("inc a", 0x3c) == PARSER_OK);
    ASSERT(asm_test("inc (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int jp_test(void)
{
    ASSERT(asm_test("jp nz,256", 0x100c2) == PARSER_OK);
    ASSERT(asm_test("jp nz,0xffff", 0xffffc2) == PARSER_OK);
    ASSERT(asm_test("jp nz,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("jp 256", 0x100c3) == PARSER_OK);
    ASSERT(asm_test("jp 0xffff", 0xffffc3) == PARSER_OK);
    ASSERT(asm_test("jp 0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("jp z,256", 0x100ca) == PARSER_OK);
    ASSERT(asm_test("jp z,0xffff", 0xffffca) == PARSER_OK);
    ASSERT(asm_test("jp z,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("jp nc,256", 0x100d2) == PARSER_OK);
    ASSERT(asm_test("jp nc,0xffff", 0xffffd2) == PARSER_OK);
    ASSERT(asm_test("jp nc,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("jp c,256", 0x100da) == PARSER_OK);
    ASSERT(asm_test("jp c,0xffff", 0xffffda) == PARSER_OK);
    ASSERT(asm_test("jp c,0x10000", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("jp (hl)", 0xe9) == PARSER_OK);
    return 0;
}

static int jr_test(void)
{
    ASSERT(asm_test("jr 16", 0x1018) == PARSER_OK);
    ASSERT(asm_test("jr -1", 0xff18) == PARSER_OK);
    ASSERT(asm_test("jr 0x100", 0) == PARSER_ERR_NUMBER_S8);
    ASSERT(asm_test("jr nz,16", 0x1020) == PARSER_OK);
    ASSERT(asm_test("jr nz,-1", 0xff20) == PARSER_OK);
    ASSERT(asm_test("jr nz,0x100", 0) == PARSER_ERR_NUMBER_S8);
    ASSERT(asm_test("jr z,16", 0x1028) == PARSER_OK);
    ASSERT(asm_test("jr z,-1", 0xff28) == PARSER_OK);
    ASSERT(asm_test("jr z,0x100", 0) == PARSER_ERR_NUMBER_S8);
    ASSERT(asm_test("jr nc,16", 0x1030) == PARSER_OK);
    ASSERT(asm_test("jr nc,-1", 0xff30) == PARSER_OK);
    ASSERT(asm_test("jr nc,0x100", 0) == PARSER_ERR_NUMBER_S8);
    ASSERT(asm_test("jr c,16", 0x1038) == PARSER_OK);
    ASSERT(asm_test("jr c,-1", 0xff38) == PARSER_OK);
    ASSERT(asm_test("jr c,0x100", 0) == PARSER_ERR_NUMBER_S8);
    return 0;
}

static int jp_label_test(void)
{
    ASSERT(asm_test("label1:\n"
                    "    jp label1",
                    0xc3) == PARSER_OK);
    ASSERT(asm_test("nop\n"
                    "label1:\n"
                    "    jp label1",
                    0x1c300) == PARSER_OK);
    ASSERT(asm_test("label1\n"
                    "    jp label1",
                    0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("jp label1\n"
                    "label1:",
                    0x3c3) == PARSER_OK);
    ASSERT(asm_test("jp invalid_label", 0xc3) == PARSER_ERR_INVALID_LABEL);

    ASSERT(asm_test("label1:\n"
                    "    jp nz,label1",
                    0xc2) == PARSER_OK);
    ASSERT(asm_test("nop\n"
                    "label1:\n"
                    "    jp nz,label1",
                    0x1c200) == PARSER_OK);
    ASSERT(asm_test("jp nz,label1\n"
                    "label1:",
                    0x3c2) == PARSER_OK);
    ASSERT(asm_test("jp nz,invalid_label", 0xc2) == PARSER_ERR_INVALID_LABEL);
    return 0;
}

static int jr_label_test(void)
{
    ASSERT(asm_test("label1:\n"
                    "    jr label1",
                    0xfe18) == PARSER_OK);
    ASSERT(asm_test("label1:\n"
                    "    nop\n"
                    "    jr label1",
                    0xfd1800) == PARSER_OK);
    ASSERT(asm_test("jr label1\n"
                    "label1:",
                    0x18) == PARSER_OK);
    ASSERT(asm_test("jr invalid_label", 0x18) == PARSER_ERR_INVALID_LABEL);

    ASSERT(asm_test("label1:\n"
                    "    jr nz,label1",
                    0xfe20) == PARSER_OK);
    ASSERT(asm_test("label1:\n"
                    "    nop\n"
                    "    jr nz,label1",
                    0xfd2000) == PARSER_OK);
    ASSERT(asm_test("jr nz,label1\n"
                    "label1:",
                    0x20) == PARSER_OK);
    ASSERT(asm_test("jr nz,invalid_label", 0x20) == PARSER_ERR_INVALID_LABEL);
    return 0;
}

static int ld_a_test(void)
{
    ASSERT(asm_test("ld a,(bc)", 0x0a) == PARSER_OK);
    ASSERT(asm_test("ld a,(de)", 0x1a) == PARSER_OK);
    ASSERT(asm_test("ld a,(hl+)", 0x7e) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld a,(hl-)", 0x7e) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld a,0", 0x3e) == PARSER_OK);
    ASSERT(asm_test("ld a,16", 0x103e) == PARSER_OK);
    ASSERT(asm_test("ld a,0xff", 0xff3e) == PARSER_OK);
    ASSERT(asm_test("ld a,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld a,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld a,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld a,b", 0x78) == PARSER_OK);
    ASSERT(asm_test("ld a,c", 0x79) == PARSER_OK);
    ASSERT(asm_test("ld a,d", 0x7a) == PARSER_OK);
    ASSERT(asm_test("ld a,e", 0x7b) == PARSER_OK);
    ASSERT(asm_test("ld a,h", 0x7c) == PARSER_OK);
    ASSERT(asm_test("ld a,l", 0x7d) == PARSER_OK);
    ASSERT(asm_test("ld a,(hl)", 0x7e) == PARSER_OK);
    ASSERT(asm_test("ld a,a", 0x7f) == PARSER_OK);
    ASSERT(asm_test("ld a,(c)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld a,(0xff00 + c)", 0xf2) == PARSER_OK);
    ASSERT(asm_test("ld a,(0xff00 + 0)", 0xf0) == PARSER_OK);
    ASSERT(asm_test("ld a,(0xff00 + 16)", 0x10f0) == PARSER_OK);
    ASSERT(asm_test("ld a,(0xff00 + 0xff)", 0xfff0) == PARSER_OK);
    ASSERT(asm_test("ld a,(0xff00 + 0x100)", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld a,(0xff01 + 0)", 0) == PARSER_ERR_NUMBER_FF00);
    ASSERT(asm_test("ld a,(0xff01)", 0xff01fa) == PARSER_OK);
    ASSERT(asm_test("ld a,(0)", 0xfa) == PARSER_OK);
    return 0;
}

static int ld_b_test(void)
{
    ASSERT(asm_test("ld b,0", 0x06) == PARSER_OK);
    ASSERT(asm_test("ld b,16", 0x1006) == PARSER_OK);
    ASSERT(asm_test("ld b,0xff", 0xff06) == PARSER_OK);
    ASSERT(asm_test("ld b,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld b,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld b,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld b,b", 0x40) == PARSER_OK);
    ASSERT(asm_test("ld b,c", 0x41) == PARSER_OK);
    ASSERT(asm_test("ld b,d", 0x42) == PARSER_OK);
    ASSERT(asm_test("ld b,e", 0x43) == PARSER_OK);
    ASSERT(asm_test("ld b,h", 0x44) == PARSER_OK);
    ASSERT(asm_test("ld b,l", 0x45) == PARSER_OK);
    ASSERT(asm_test("ld b,(hl)", 0x46) == PARSER_OK);
    ASSERT(asm_test("ld b,a", 0x47) == PARSER_OK);
    return 0;
}

static int ld_c_test(void)
{
    ASSERT(asm_test("ld c,0", 0x0e) == PARSER_OK);
    ASSERT(asm_test("ld c,16", 0x100e) == PARSER_OK);
    ASSERT(asm_test("ld c,0xff", 0xff0e) == PARSER_OK);
    ASSERT(asm_test("ld c,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld c,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld c,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld c,b", 0x48) == PARSER_OK);
    ASSERT(asm_test("ld c,c", 0x49) == PARSER_OK);
    ASSERT(asm_test("ld c,d", 0x4a) == PARSER_OK);
    ASSERT(asm_test("ld c,e", 0x4b) == PARSER_OK);
    ASSERT(asm_test("ld c,h", 0x4c) == PARSER_OK);
    ASSERT(asm_test("ld c,l", 0x4d) == PARSER_OK);
    ASSERT(asm_test("ld c,(hl)", 0x4e) == PARSER_OK);
    ASSERT(asm_test("ld c,a", 0x4f) == PARSER_OK);
    return 0;
}

static int ld_d_test(void)
{
    ASSERT(asm_test("ld d,0", 0x16) == PARSER_OK);
    ASSERT(asm_test("ld d,16", 0x1016) == PARSER_OK);
    ASSERT(asm_test("ld d,0xff", 0xff16) == PARSER_OK);
    ASSERT(asm_test("ld d,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld d,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld d,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld d,b", 0x50) == PARSER_OK);
    ASSERT(asm_test("ld d,c", 0x51) == PARSER_OK);
    ASSERT(asm_test("ld d,d", 0x52) == PARSER_OK);
    ASSERT(asm_test("ld d,e", 0x53) == PARSER_OK);
    ASSERT(asm_test("ld d,h", 0x54) == PARSER_OK);
    ASSERT(asm_test("ld d,l", 0x55) == PARSER_OK);
    ASSERT(asm_test("ld d,(hl)", 0x56) == PARSER_OK);
    ASSERT(asm_test("ld d,a", 0x57) == PARSER_OK);
    return 0;
}

static int ld_e_test(void)
{
    ASSERT(asm_test("ld e,0", 0x1e) == PARSER_OK);
    ASSERT(asm_test("ld e,16", 0x101e) == PARSER_OK);
    ASSERT(asm_test("ld e,0xff", 0xff1e) == PARSER_OK);
    ASSERT(asm_test("ld e,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld e,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld e,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld e,b", 0x58) == PARSER_OK);
    ASSERT(asm_test("ld e,c", 0x59) == PARSER_OK);
    ASSERT(asm_test("ld e,d", 0x5a) == PARSER_OK);
    ASSERT(asm_test("ld e,e", 0x5b) == PARSER_OK);
    ASSERT(asm_test("ld e,h", 0x5c) == PARSER_OK);
    ASSERT(asm_test("ld e,l", 0x5d) == PARSER_OK);
    ASSERT(asm_test("ld e,(hl)", 0x5e) == PARSER_OK);
    ASSERT(asm_test("ld e,a", 0x5f) == PARSER_OK);
    return 0;
}

static int ld_h_test(void)
{
    ASSERT(asm_test("ld h,0", 0x26) == PARSER_OK);
    ASSERT(asm_test("ld h,16", 0x1026) == PARSER_OK);
    ASSERT(asm_test("ld h,0xff", 0xff26) == PARSER_OK);
    ASSERT(asm_test("ld h,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld h,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld h,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld h,b", 0x60) == PARSER_OK);
    ASSERT(asm_test("ld h,c", 0x61) == PARSER_OK);
    ASSERT(asm_test("ld h,d", 0x62) == PARSER_OK);
    ASSERT(asm_test("ld h,e", 0x63) == PARSER_OK);
    ASSERT(asm_test("ld h,h", 0x64) == PARSER_OK);
    ASSERT(asm_test("ld h,l", 0x65) == PARSER_OK);
    ASSERT(asm_test("ld h,(hl)", 0x66) == PARSER_OK);
    ASSERT(asm_test("ld h,a", 0x67) == PARSER_OK);
    return 0;
}

static int ld_l_test(void)
{
    ASSERT(asm_test("ld l,0", 0x2e) == PARSER_OK);
    ASSERT(asm_test("ld l,16", 0x102e) == PARSER_OK);
    ASSERT(asm_test("ld l,0xff", 0xff2e) == PARSER_OK);
    ASSERT(asm_test("ld l,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld l,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld l,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld l,b", 0x68) == PARSER_OK);
    ASSERT(asm_test("ld l,c", 0x69) == PARSER_OK);
    ASSERT(asm_test("ld l,d", 0x6a) == PARSER_OK);
    ASSERT(asm_test("ld l,e", 0x6b) == PARSER_OK);
    ASSERT(asm_test("ld l,h", 0x6c) == PARSER_OK);
    ASSERT(asm_test("ld l,l", 0x6d) == PARSER_OK);
    ASSERT(asm_test("ld l,(hl)", 0x6e) == PARSER_OK);
    ASSERT(asm_test("ld l,a", 0x6f) == PARSER_OK);
    return 0;
}

static int ld_reg16_test(void)
{
    ASSERT(asm_test("ld bc,0", 0x01) == PARSER_OK);
    ASSERT(asm_test("ld bc,0xffff", 0xffff01) == PARSER_OK);
    ASSERT(asm_test("ld bc,(10)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld de,0", 0x11) == PARSER_OK);
    ASSERT(asm_test("ld de,0xffff", 0xffff11) == PARSER_OK);
    ASSERT(asm_test("ld de,(10)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld hl,0", 0x21) == PARSER_OK);
    ASSERT(asm_test("ld hl,0xffff", 0xffff21) == PARSER_OK);
    ASSERT(asm_test("ld hl,(10)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld hl,sp+0", 0xf8) == PARSER_OK);
    ASSERT(asm_test("ld hl,sp + 0x10", 0x10f8) == PARSER_OK);
    ASSERT(asm_test("ld hl,sp + 16", 0x10f8) == PARSER_OK);
    ASSERT(asm_test("ld hl,sp + -16", 0xf0f8) == PARSER_OK);
    ASSERT(asm_test("ld hl,sp + -128", 0x80f8) == PARSER_OK);
    ASSERT(asm_test("ld hl,sp + 127", 0x7ff8) == PARSER_OK);
    ASSERT(asm_test("ld sp,hl", 0xf9) == PARSER_OK);
    return 0;
}

static int ld_store_test(void)
{
    ASSERT(asm_test("ld (bc),a", 0x02) == PARSER_OK);
    ASSERT(asm_test("ld (de),a", 0x12) == PARSER_OK);
    ASSERT(asm_test("ld (hl),0", 0x36) == PARSER_OK);
    ASSERT(asm_test("ld (hl),16", 0x1036) == PARSER_OK);
    ASSERT(asm_test("ld (hl),0xff", 0xff36) == PARSER_OK);
    ASSERT(asm_test("ld (hl),0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld (hl),b", 0x70) == PARSER_OK);
    ASSERT(asm_test("ld (hl),c", 0x71) == PARSER_OK);
    ASSERT(asm_test("ld (hl),d", 0x72) == PARSER_OK);
    ASSERT(asm_test("ld (hl),e", 0x73) == PARSER_OK);
    ASSERT(asm_test("ld (hl),h", 0x74) == PARSER_OK);
    ASSERT(asm_test("ld (hl),l", 0x75) == PARSER_OK);
    ASSERT(asm_test("ld (hl),a", 0x77) == PARSER_OK);
    ASSERT(asm_test("ld (0),a", 0xea) == PARSER_OK);
    ASSERT(asm_test("ld (16),a", 0x10ea) == PARSER_OK);
    ASSERT(asm_test("ld (0x10),a", 0x10ea) == PARSER_OK);
    ASSERT(asm_test("ld (0xffff),a", 0xffffea) == PARSER_OK);
    ASSERT(asm_test("ld (0),sp", 0x08) == PARSER_OK);
    ASSERT(asm_test("ld (16),sp", 0x1008) == PARSER_OK);
    ASSERT(asm_test("ld (0x10),sp", 0x1008) == PARSER_OK);
    ASSERT(asm_test("ld (0xffff),sp", 0xffff08) == PARSER_OK);
    ASSERT(asm_test("ld (0),b", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld (0xff00 + c),a", 0xe2) == PARSER_OK);
    ASSERT(asm_test("ld (0xff00 + c),b", 0xe2) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ld (0xff00 + 0),a", 0xe0) == PARSER_OK);
    ASSERT(asm_test("ld (0xff00 + 16),a", 0x10e0) == PARSER_OK);
    ASSERT(asm_test("ld (0xff00 + 0x10),a", 0x10e0) == PARSER_OK);
    ASSERT(asm_test("ld (0xff00 + 0xff),a", 0xffe0) == PARSER_OK);
    ASSERT(asm_test("ld (0xff00 + 0x100),a", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("ld (0xff00 + 0xff),b", 0xffe0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int ldi_test(void)
{
    ASSERT(asm_test("ldi (hl),a", 0x22) == PARSER_OK);
    ASSERT(asm_test("ldi (de),a", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldi (hl),b", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldi a,(hl)", 0x2a) == PARSER_OK);
    ASSERT(asm_test("ldi a,(de)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldi b,(hl)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int ldd_test(void)
{
    ASSERT(asm_test("ldd (hl),a", 0x32) == PARSER_OK);
    ASSERT(asm_test("ldd (de),a", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldd (hl),b", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldd a,(hl)", 0x3a) == PARSER_OK);
    ASSERT(asm_test("ldd a,(de)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("ldd b,(hl)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int or_test(void)
{
    ASSERT(asm_test("or b", 0xb0) == PARSER_OK);
    ASSERT(asm_test("or c", 0xb1) == PARSER_OK);
    ASSERT(asm_test("or d", 0xb2) == PARSER_OK);
    ASSERT(asm_test("or e", 0xb3) == PARSER_OK);
    ASSERT(asm_test("or h", 0xb4) == PARSER_OK);
    ASSERT(asm_test("or l", 0xb5) == PARSER_OK);
    ASSERT(asm_test("or (hl)", 0xb6) == PARSER_OK);
    ASSERT(asm_test("or a", 0xb7) == PARSER_OK);
    ASSERT(asm_test("or 0", 0xf6) == PARSER_OK);
    ASSERT(asm_test("or 16", 0x10f6) == PARSER_OK);
    ASSERT(asm_test("or 0xff", 0xfff6) == PARSER_OK);
    ASSERT(asm_test("or 0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("or 256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("or x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("or (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int pop_test(void)
{
    ASSERT(asm_test("pop bc", 0xc1) == PARSER_OK);
    ASSERT(asm_test("pop de", 0xd1) == PARSER_OK);
    ASSERT(asm_test("pop hl", 0xe1) == PARSER_OK);
    ASSERT(asm_test("pop af", 0xf1) == PARSER_OK);
    return 0;
}

static int push_test(void)
{
    ASSERT(asm_test("push bc", 0xc5) == PARSER_OK);
    ASSERT(asm_test("push de", 0xd5) == PARSER_OK);
    ASSERT(asm_test("push hl", 0xe5) == PARSER_OK);
    ASSERT(asm_test("push af", 0xf5) == PARSER_OK);
    return 0;
}

static int res_test(void)
{
    ASSERT(asm_test("res 0,b", 0x80cb) == PARSER_OK);
    ASSERT(asm_test("res 0,c", 0x81cb) == PARSER_OK);
    ASSERT(asm_test("res 0,d", 0x82cb) == PARSER_OK);
    ASSERT(asm_test("res 0,e", 0x83cb) == PARSER_OK);
    ASSERT(asm_test("res 0,h", 0x84cb) == PARSER_OK);
    ASSERT(asm_test("res 0,l", 0x85cb) == PARSER_OK);
    ASSERT(asm_test("res 0,(hl)", 0x86cb) == PARSER_OK);
    ASSERT(asm_test("res 0,a", 0x87cb) == PARSER_OK);
    ASSERT(asm_test("res 1,b", 0x88cb) == PARSER_OK);
    ASSERT(asm_test("res 1,c", 0x89cb) == PARSER_OK);
    ASSERT(asm_test("res 1,d", 0x8acb) == PARSER_OK);
    ASSERT(asm_test("res 1,e", 0x8bcb) == PARSER_OK);
    ASSERT(asm_test("res 1,h", 0x8ccb) == PARSER_OK);
    ASSERT(asm_test("res 1,l", 0x8dcb) == PARSER_OK);
    ASSERT(asm_test("res 1,(hl)", 0x8ecb) == PARSER_OK);
    ASSERT(asm_test("res 1,a", 0x8fcb) == PARSER_OK);
    ASSERT(asm_test("res 2,b", 0x90cb) == PARSER_OK);
    ASSERT(asm_test("res 2,c", 0x91cb) == PARSER_OK);
    ASSERT(asm_test("res 2,d", 0x92cb) == PARSER_OK);
    ASSERT(asm_test("res 2,e", 0x93cb) == PARSER_OK);
    ASSERT(asm_test("res 2,h", 0x94cb) == PARSER_OK);
    ASSERT(asm_test("res 2,l", 0x95cb) == PARSER_OK);
    ASSERT(asm_test("res 2,(hl)", 0x96cb) == PARSER_OK);
    ASSERT(asm_test("res 2,a", 0x97cb) == PARSER_OK);
    ASSERT(asm_test("res 3,b", 0x98cb) == PARSER_OK);
    ASSERT(asm_test("res 3,c", 0x99cb) == PARSER_OK);
    ASSERT(asm_test("res 3,d", 0x9acb) == PARSER_OK);
    ASSERT(asm_test("res 3,e", 0x9bcb) == PARSER_OK);
    ASSERT(asm_test("res 3,h", 0x9ccb) == PARSER_OK);
    ASSERT(asm_test("res 3,l", 0x9dcb) == PARSER_OK);
    ASSERT(asm_test("res 3,(hl)", 0x9ecb) == PARSER_OK);
    ASSERT(asm_test("res 3,a", 0x9fcb) == PARSER_OK);
    ASSERT(asm_test("res 4,b", 0xa0cb) == PARSER_OK);
    ASSERT(asm_test("res 4,c", 0xa1cb) == PARSER_OK);
    ASSERT(asm_test("res 4,d", 0xa2cb) == PARSER_OK);
    ASSERT(asm_test("res 4,e", 0xa3cb) == PARSER_OK);
    ASSERT(asm_test("res 4,h", 0xa4cb) == PARSER_OK);
    ASSERT(asm_test("res 4,l", 0xa5cb) == PARSER_OK);
    ASSERT(asm_test("res 4,(hl)", 0xa6cb) == PARSER_OK);
    ASSERT(asm_test("res 4,a", 0xa7cb) == PARSER_OK);
    ASSERT(asm_test("res 5,b", 0xa8cb) == PARSER_OK);
    ASSERT(asm_test("res 5,c", 0xa9cb) == PARSER_OK);
    ASSERT(asm_test("res 5,d", 0xaacb) == PARSER_OK);
    ASSERT(asm_test("res 5,e", 0xabcb) == PARSER_OK);
    ASSERT(asm_test("res 5,h", 0xaccb) == PARSER_OK);
    ASSERT(asm_test("res 5,l", 0xadcb) == PARSER_OK);
    ASSERT(asm_test("res 5,(hl)", 0xaecb) == PARSER_OK);
    ASSERT(asm_test("res 5,a", 0xafcb) == PARSER_OK);
    ASSERT(asm_test("res 6,b", 0xb0cb) == PARSER_OK);
    ASSERT(asm_test("res 6,c", 0xb1cb) == PARSER_OK);
    ASSERT(asm_test("res 6,d", 0xb2cb) == PARSER_OK);
    ASSERT(asm_test("res 6,e", 0xb3cb) == PARSER_OK);
    ASSERT(asm_test("res 6,h", 0xb4cb) == PARSER_OK);
    ASSERT(asm_test("res 6,l", 0xb5cb) == PARSER_OK);
    ASSERT(asm_test("res 6,(hl)", 0xb6cb) == PARSER_OK);
    ASSERT(asm_test("res 6,a", 0xb7cb) == PARSER_OK);
    ASSERT(asm_test("res 7,b", 0xb8cb) == PARSER_OK);
    ASSERT(asm_test("res 7,c", 0xb9cb) == PARSER_OK);
    ASSERT(asm_test("res 7,d", 0xbacb) == PARSER_OK);
    ASSERT(asm_test("res 7,e", 0xbbcb) == PARSER_OK);
    ASSERT(asm_test("res 7,h", 0xbccb) == PARSER_OK);
    ASSERT(asm_test("res 7,l", 0xbdcb) == PARSER_OK);
    ASSERT(asm_test("res 7,(hl)", 0xbecb) == PARSER_OK);
    ASSERT(asm_test("res 7,a", 0xbfcb) == PARSER_OK);
    ASSERT(asm_test("res 8,b", 0) == PARSER_ERR_NUMBER);
    return 0;
}

static int ret_test(void)
{
    ASSERT(asm_test("ret nz", 0xc0) == PARSER_OK);
    ASSERT(asm_test("ret z", 0xc8) == PARSER_OK);
    ASSERT(asm_test("ret", 0xc9) == PARSER_OK);
    ASSERT(asm_test("ret nc", 0xd0) == PARSER_OK);
    ASSERT(asm_test("ret c", 0xd8) == PARSER_OK);
    return 0;
}

static int rl_test(void)
{
    ASSERT(asm_test("rl b", 0x10cb) == PARSER_OK);
    ASSERT(asm_test("rl c", 0x11cb) == PARSER_OK);
    ASSERT(asm_test("rl d", 0x12cb) == PARSER_OK);
    ASSERT(asm_test("rl e", 0x13cb) == PARSER_OK);
    ASSERT(asm_test("rl h", 0x14cb) == PARSER_OK);
    ASSERT(asm_test("rl l", 0x15cb) == PARSER_OK);
    ASSERT(asm_test("rl (hl)", 0x16cb) == PARSER_OK);
    ASSERT(asm_test("rl a", 0x17cb) == PARSER_OK);
    return 0;
}

static int rlc_test(void)
{
    ASSERT(asm_test("rlc b", 0x00cb) == PARSER_OK);
    ASSERT(asm_test("rlc c", 0x01cb) == PARSER_OK);
    ASSERT(asm_test("rlc d", 0x02cb) == PARSER_OK);
    ASSERT(asm_test("rlc e", 0x03cb) == PARSER_OK);
    ASSERT(asm_test("rlc h", 0x04cb) == PARSER_OK);
    ASSERT(asm_test("rlc l", 0x05cb) == PARSER_OK);
    ASSERT(asm_test("rlc (hl)", 0x06cb) == PARSER_OK);
    ASSERT(asm_test("rlc a", 0x07cb) == PARSER_OK);
    return 0;
}

static int rr_test(void)
{
    ASSERT(asm_test("rr b", 0x18cb) == PARSER_OK);
    ASSERT(asm_test("rr c", 0x19cb) == PARSER_OK);
    ASSERT(asm_test("rr d", 0x1acb) == PARSER_OK);
    ASSERT(asm_test("rr e", 0x1bcb) == PARSER_OK);
    ASSERT(asm_test("rr h", 0x1ccb) == PARSER_OK);
    ASSERT(asm_test("rr l", 0x1dcb) == PARSER_OK);
    ASSERT(asm_test("rr (hl)", 0x1ecb) == PARSER_OK);
    ASSERT(asm_test("rr a", 0x1fcb) == PARSER_OK);
    return 0;
}

static int rrc_test(void)
{
    ASSERT(asm_test("rrc b", 0x08cb) == PARSER_OK);
    ASSERT(asm_test("rrc c", 0x09cb) == PARSER_OK);
    ASSERT(asm_test("rrc d", 0x0acb) == PARSER_OK);
    ASSERT(asm_test("rrc e", 0x0bcb) == PARSER_OK);
    ASSERT(asm_test("rrc h", 0x0ccb) == PARSER_OK);
    ASSERT(asm_test("rrc l", 0x0dcb) == PARSER_OK);
    ASSERT(asm_test("rrc (hl)", 0x0ecb) == PARSER_OK);
    ASSERT(asm_test("rrc a", 0x0fcb) == PARSER_OK);
    return 0;
}

static int rst_test(void)
{
    ASSERT(asm_test("rst 0x00", 0xc7) == PARSER_OK);
    ASSERT(asm_test("rst 0x08", 0xcf) == PARSER_OK);
    ASSERT(asm_test("rst 0x10", 0xd7) == PARSER_OK);
    ASSERT(asm_test("rst 0x18", 0xdf) == PARSER_OK);
    ASSERT(asm_test("rst 0x20", 0xe7) == PARSER_OK);
    ASSERT(asm_test("rst 0x28", 0xef) == PARSER_OK);
    ASSERT(asm_test("rst 0x30", 0xf7) == PARSER_OK);
    ASSERT(asm_test("rst 0x38", 0xff) == PARSER_OK);
    ASSERT(asm_test("rst 0x40", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int sbc_test(void)
{
    ASSERT(asm_test("sbc a,b", 0x98) == PARSER_OK);
    ASSERT(asm_test("sbc a,c", 0x99) == PARSER_OK);
    ASSERT(asm_test("sbc a,d", 0x9a) == PARSER_OK);
    ASSERT(asm_test("sbc a,e", 0x9b) == PARSER_OK);
    ASSERT(asm_test("sbc a,h", 0x9c) == PARSER_OK);
    ASSERT(asm_test("sbc a,l", 0x9d) == PARSER_OK);
    ASSERT(asm_test("sbc a,(hl)", 0x9e) == PARSER_OK);
    ASSERT(asm_test("sbc a,a", 0x9f) == PARSER_OK);
    ASSERT(asm_test("sbc a,0", 0xde) == PARSER_OK);
    ASSERT(asm_test("sbc a,16", 0x10de) == PARSER_OK);
    ASSERT(asm_test("sbc a,0xff", 0xffde) == PARSER_OK);
    ASSERT(asm_test("sbc a,0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("sbc a,256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("sbc a,x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("sbc a,(bc)", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("sbc b,a", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int set_test(void)
{
    ASSERT(asm_test("set 0,b", 0xc0cb) == PARSER_OK);
    ASSERT(asm_test("set 0,c", 0xc1cb) == PARSER_OK);
    ASSERT(asm_test("set 0,d", 0xc2cb) == PARSER_OK);
    ASSERT(asm_test("set 0,e", 0xc3cb) == PARSER_OK);
    ASSERT(asm_test("set 0,h", 0xc4cb) == PARSER_OK);
    ASSERT(asm_test("set 0,l", 0xc5cb) == PARSER_OK);
    ASSERT(asm_test("set 0,(hl)", 0xc6cb) == PARSER_OK);
    ASSERT(asm_test("set 0,a", 0xc7cb) == PARSER_OK);
    ASSERT(asm_test("set 1,b", 0xc8cb) == PARSER_OK);
    ASSERT(asm_test("set 1,c", 0xc9cb) == PARSER_OK);
    ASSERT(asm_test("set 1,d", 0xcacb) == PARSER_OK);
    ASSERT(asm_test("set 1,e", 0xcbcb) == PARSER_OK);
    ASSERT(asm_test("set 1,h", 0xcccb) == PARSER_OK);
    ASSERT(asm_test("set 1,l", 0xcdcb) == PARSER_OK);
    ASSERT(asm_test("set 1,(hl)", 0xcecb) == PARSER_OK);
    ASSERT(asm_test("set 1,a", 0xcfcb) == PARSER_OK);
    ASSERT(asm_test("set 2,b", 0xd0cb) == PARSER_OK);
    ASSERT(asm_test("set 2,c", 0xd1cb) == PARSER_OK);
    ASSERT(asm_test("set 2,d", 0xd2cb) == PARSER_OK);
    ASSERT(asm_test("set 2,e", 0xd3cb) == PARSER_OK);
    ASSERT(asm_test("set 2,h", 0xd4cb) == PARSER_OK);
    ASSERT(asm_test("set 2,l", 0xd5cb) == PARSER_OK);
    ASSERT(asm_test("set 2,(hl)", 0xd6cb) == PARSER_OK);
    ASSERT(asm_test("set 2,a", 0xd7cb) == PARSER_OK);
    ASSERT(asm_test("set 3,b", 0xd8cb) == PARSER_OK);
    ASSERT(asm_test("set 3,c", 0xd9cb) == PARSER_OK);
    ASSERT(asm_test("set 3,d", 0xdacb) == PARSER_OK);
    ASSERT(asm_test("set 3,e", 0xdbcb) == PARSER_OK);
    ASSERT(asm_test("set 3,h", 0xdccb) == PARSER_OK);
    ASSERT(asm_test("set 3,l", 0xddcb) == PARSER_OK);
    ASSERT(asm_test("set 3,(hl)", 0xdecb) == PARSER_OK);
    ASSERT(asm_test("set 3,a", 0xdfcb) == PARSER_OK);
    ASSERT(asm_test("set 4,b", 0xe0cb) == PARSER_OK);
    ASSERT(asm_test("set 4,c", 0xe1cb) == PARSER_OK);
    ASSERT(asm_test("set 4,d", 0xe2cb) == PARSER_OK);
    ASSERT(asm_test("set 4,e", 0xe3cb) == PARSER_OK);
    ASSERT(asm_test("set 4,h", 0xe4cb) == PARSER_OK);
    ASSERT(asm_test("set 4,l", 0xe5cb) == PARSER_OK);
    ASSERT(asm_test("set 4,(hl)", 0xe6cb) == PARSER_OK);
    ASSERT(asm_test("set 4,a", 0xe7cb) == PARSER_OK);
    ASSERT(asm_test("set 5,b", 0xe8cb) == PARSER_OK);
    ASSERT(asm_test("set 5,c", 0xe9cb) == PARSER_OK);
    ASSERT(asm_test("set 5,d", 0xeacb) == PARSER_OK);
    ASSERT(asm_test("set 5,e", 0xebcb) == PARSER_OK);
    ASSERT(asm_test("set 5,h", 0xeccb) == PARSER_OK);
    ASSERT(asm_test("set 5,l", 0xedcb) == PARSER_OK);
    ASSERT(asm_test("set 5,(hl)", 0xeecb) == PARSER_OK);
    ASSERT(asm_test("set 5,a", 0xefcb) == PARSER_OK);
    ASSERT(asm_test("set 6,b", 0xf0cb) == PARSER_OK);
    ASSERT(asm_test("set 6,c", 0xf1cb) == PARSER_OK);
    ASSERT(asm_test("set 6,d", 0xf2cb) == PARSER_OK);
    ASSERT(asm_test("set 6,e", 0xf3cb) == PARSER_OK);
    ASSERT(asm_test("set 6,h", 0xf4cb) == PARSER_OK);
    ASSERT(asm_test("set 6,l", 0xf5cb) == PARSER_OK);
    ASSERT(asm_test("set 6,(hl)", 0xf6cb) == PARSER_OK);
    ASSERT(asm_test("set 6,a", 0xf7cb) == PARSER_OK);
    ASSERT(asm_test("set 7,b", 0xf8cb) == PARSER_OK);
    ASSERT(asm_test("set 7,c", 0xf9cb) == PARSER_OK);
    ASSERT(asm_test("set 7,d", 0xfacb) == PARSER_OK);
    ASSERT(asm_test("set 7,e", 0xfbcb) == PARSER_OK);
    ASSERT(asm_test("set 7,h", 0xfccb) == PARSER_OK);
    ASSERT(asm_test("set 7,l", 0xfdcb) == PARSER_OK);
    ASSERT(asm_test("set 7,(hl)", 0xfecb) == PARSER_OK);
    ASSERT(asm_test("set 7,a", 0xffcb) == PARSER_OK);
    ASSERT(asm_test("set 8,b", 0) == PARSER_ERR_NUMBER);
    return 0;
}

static int sla_test(void)
{
    ASSERT(asm_test("sla b", 0x20cb) == PARSER_OK);
    ASSERT(asm_test("sla c", 0x21cb) == PARSER_OK);
    ASSERT(asm_test("sla d", 0x22cb) == PARSER_OK);
    ASSERT(asm_test("sla e", 0x23cb) == PARSER_OK);
    ASSERT(asm_test("sla h", 0x24cb) == PARSER_OK);
    ASSERT(asm_test("sla l", 0x25cb) == PARSER_OK);
    ASSERT(asm_test("sla (hl)", 0x26cb) == PARSER_OK);
    ASSERT(asm_test("sla a", 0x27cb) == PARSER_OK);
    return 0;
}

static int sra_test(void)
{
    ASSERT(asm_test("sra b", 0x28cb) == PARSER_OK);
    ASSERT(asm_test("sra c", 0x29cb) == PARSER_OK);
    ASSERT(asm_test("sra d", 0x2acb) == PARSER_OK);
    ASSERT(asm_test("sra e", 0x2bcb) == PARSER_OK);
    ASSERT(asm_test("sra h", 0x2ccb) == PARSER_OK);
    ASSERT(asm_test("sra l", 0x2dcb) == PARSER_OK);
    ASSERT(asm_test("sra (hl)", 0x2ecb) == PARSER_OK);
    ASSERT(asm_test("sra a", 0x2fcb) == PARSER_OK);
    return 0;
}

static int srl_test(void)
{
    ASSERT(asm_test("srl b", 0x38cb) == PARSER_OK);
    ASSERT(asm_test("srl c", 0x39cb) == PARSER_OK);
    ASSERT(asm_test("srl d", 0x3acb) == PARSER_OK);
    ASSERT(asm_test("srl e", 0x3bcb) == PARSER_OK);
    ASSERT(asm_test("srl h", 0x3ccb) == PARSER_OK);
    ASSERT(asm_test("srl l", 0x3dcb) == PARSER_OK);
    ASSERT(asm_test("srl (hl)", 0x3ecb) == PARSER_OK);
    ASSERT(asm_test("srl a", 0x3fcb) == PARSER_OK);
    return 0;
}

static int sub_test(void)
{
    ASSERT(asm_test("sub b", 0x90) == PARSER_OK);
    ASSERT(asm_test("sub c", 0x91) == PARSER_OK);
    ASSERT(asm_test("sub d", 0x92) == PARSER_OK);
    ASSERT(asm_test("sub e", 0x93) == PARSER_OK);
    ASSERT(asm_test("sub h", 0x94) == PARSER_OK);
    ASSERT(asm_test("sub l", 0x95) == PARSER_OK);
    ASSERT(asm_test("sub (hl)", 0x96) == PARSER_OK);
    ASSERT(asm_test("sub a", 0x97) == PARSER_OK);
    ASSERT(asm_test("sub 0", 0xd6) == PARSER_OK);
    ASSERT(asm_test("sub 16", 0x10d6) == PARSER_OK);
    ASSERT(asm_test("sub 0xff", 0xffd6) == PARSER_OK);
    ASSERT(asm_test("sub 0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("sub 256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("sub x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("sub (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int swap_test(void)
{
    ASSERT(asm_test("swap b", 0x30cb) == PARSER_OK);
    ASSERT(asm_test("swap c", 0x31cb) == PARSER_OK);
    ASSERT(asm_test("swap d", 0x32cb) == PARSER_OK);
    ASSERT(asm_test("swap e", 0x33cb) == PARSER_OK);
    ASSERT(asm_test("swap h", 0x34cb) == PARSER_OK);
    ASSERT(asm_test("swap l", 0x35cb) == PARSER_OK);
    ASSERT(asm_test("swap (hl)", 0x36cb) == PARSER_OK);
    ASSERT(asm_test("swap a", 0x37cb) == PARSER_OK);
    return 0;
}

static int xor_test(void)
{
    ASSERT(asm_test("xor b", 0xa8) == PARSER_OK);
    ASSERT(asm_test("xor c", 0xa9) == PARSER_OK);
    ASSERT(asm_test("xor d", 0xaa) == PARSER_OK);
    ASSERT(asm_test("xor e", 0xab) == PARSER_OK);
    ASSERT(asm_test("xor h", 0xac) == PARSER_OK);
    ASSERT(asm_test("xor l", 0xad) == PARSER_OK);
    ASSERT(asm_test("xor (hl)", 0xae) == PARSER_OK);
    ASSERT(asm_test("xor a", 0xaf) == PARSER_OK);
    ASSERT(asm_test("xor 0", 0xee) == PARSER_OK);
    ASSERT(asm_test("xor 16", 0x10ee) == PARSER_OK);
    ASSERT(asm_test("xor 0xff", 0xffee) == PARSER_OK);
    ASSERT(asm_test("xor 0x100", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("xor 256", 0) == PARSER_ERR_NUMBER);
    ASSERT(asm_test("xor x", 0) == PARSER_ERR_SYNTAX);
    ASSERT(asm_test("xor (bc)", 0) == PARSER_ERR_SYNTAX);
    return 0;
}

static int other_test(void)
{
    ASSERT(asm_test("ccf", 0x3f) == PARSER_OK);
    ASSERT(asm_test("cpl", 0x2f) == PARSER_OK);
    ASSERT(asm_test("daa", 0x27) == PARSER_OK);
    ASSERT(asm_test("di", 0xf3) == PARSER_OK);
    ASSERT(asm_test("ei", 0xfb) == PARSER_OK);
    ASSERT(asm_test("halt", 0x76) == PARSER_OK);
    ASSERT(asm_test("nop", 0x00) == PARSER_OK);
    ASSERT(asm_test("reti", 0xd9) == PARSER_OK);
    ASSERT(asm_test("rla", 0x17) == PARSER_OK);
    ASSERT(asm_test("rlca", 0x07) == PARSER_OK);
    ASSERT(asm_test("rra", 0x1f) == PARSER_OK);
    ASSERT(asm_test("rrca", 0x0f) == PARSER_OK);
    ASSERT(asm_test("scf", 0x37) == PARSER_OK);
    ASSERT(asm_test("stop", 0x10) == PARSER_OK);
    return 0;
}

void parser_test(void)
{
    ut_run(adc_test);
    ut_run(add_test);
    ut_run(and_test);
    ut_run(bit_test);
    ut_run(call_test);
    ut_run(cp_test);
    ut_run(db_test);
    ut_run(dec_test);
    ut_run(inc_test);
    ut_run(jp_test);
    ut_run(jr_test);
    ut_run(jp_label_test);
    ut_run(jr_label_test);
    ut_run(ld_a_test);
    ut_run(ld_b_test);
    ut_run(ld_c_test);
    ut_run(ld_d_test);
    ut_run(ld_e_test);
    ut_run(ld_h_test);
    ut_run(ld_l_test);
    ut_run(ld_reg16_test);
    ut_run(ld_store_test);
    ut_run(ldi_test);
    ut_run(ldd_test);
    ut_run(or_test);
    ut_run(pop_test);
    ut_run(push_test);
    ut_run(res_test);
    ut_run(ret_test);
    ut_run(rl_test);
    ut_run(rlc_test);
    ut_run(rr_test);
    ut_run(rrc_test);
    ut_run(rst_test);
    ut_run(sbc_test);
    ut_run(set_test);
    ut_run(sla_test);
    ut_run(sra_test);
    ut_run(srl_test);
    ut_run(sub_test);
    ut_run(swap_test);
    ut_run(xor_test);
    ut_run(other_test);
}
