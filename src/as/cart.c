#include "cart.h"
#include <string.h>

void cart_header_init(cart_header_t *header, const char *title)
{
    memset(header, 0, sizeof(cart_header_t));
    snprintf((char *)header->title, sizeof(header->title), "%s", title);
    header->cgb = 0;
    header->sgb = 0;
    header->cart_type = 0;
    header->rom_size = 0;
    header->ram_size = 0;
    header->country_code = 1;
    header->licensee_old = 1;
    header->mask_rom = 0;
}

const uint8_t logo[] = {
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63,
    0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e};

static int cart_write_logo(FILE *output)
{
    int ret = fseek(output, 0x0104, SEEK_SET);
    if (ret != 0) {
        fprintf(stderr, "ERROR: could not seek to 0x0104\n");
        return -1;
    }
    size_t size = fwrite(logo, sizeof(logo), 1, output);
    if (size != 1) {
        fprintf(stderr, "ERROR: could not write logo\n");
        return -1;
    }
    return 0;
}

static uint8_t cart_header_checksum(uint8_t *header)
{
    uint8_t header_checksum = 0;
    for (size_t i = 0; i < sizeof(cart_header_t) - 3; ++i) {
        header_checksum -= header[i] + 1;
    }
    return header_checksum;
}

static uint16_t cart_global_checksum(uint8_t *header, FILE *output)
{
    long rom_size = ftell(output);
    int ret = fseek(output, 0L, SEEK_SET);
    if (ret != 0) {
        fprintf(stderr, "ERROR: could not seek to beginning\n");
        return 0;
    }
    uint16_t global_checksum = 0;
    for (int i = 0; i < rom_size; ++i) {
        if (i >= 0x134 && i <= 0x14d) {
            global_checksum += header[i - 0x134];
            continue;
        } else if (i >= 0x14e && i <= 0x14f) {
            continue;
        }
        uint8_t byte;
        ret = fread(&byte, 1, 1, output);
        if (ret != 1) {
            fprintf(stderr, "ERROR: could not read byte %d\n", i);
            return 0;
        }
        global_checksum += byte;
    }
    return global_checksum;
}

int cart_header_write(cart_header_t *ch, FILE *output)
{
    uint8_t *header = (uint8_t *)ch;
    int ret = cart_write_logo(output);
    if (ret < 0) {
        return -1;
    }
    ch->compl_check = cart_header_checksum(header);
    ret = fseek(output, 0L, SEEK_END);
    if (ret != 0) {
        fprintf(stderr, "ERROR: could not seek to end\n");
        return -1;
    }
    uint16_t global_checksum = cart_global_checksum(header, output);
    ch->checksum_h = (global_checksum & 0xff00) >> 8;
    ch->checksum_l = global_checksum & 0xff;
    ret = fseek(output, 0x0134, SEEK_SET);
    if (ret != 0) {
        fprintf(stderr, "ERROR: could not seek to 0x0134\n");
        return -1;
    }
    ret = fwrite(header, sizeof(cart_header_t), 1, output);
    if (ret < 0) {
        fprintf(stderr, "ERROR: could not write header\n");
        return -1;
    }
    return 0;
}
