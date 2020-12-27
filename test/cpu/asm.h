#ifndef TEST_ASM_H
#define TEST_ASM_H

#include <stddef.h>
#include <stdint.h>

int asm_test(const char *src, uint32_t opcode);
int asm_to_opcode(const char *src, void *buf, size_t buf_size);

#endif /* !TEST_ASM_H */
