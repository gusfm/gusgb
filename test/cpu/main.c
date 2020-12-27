#include "ut.h"

struct ut unit_test;

extern void parser_test(void);
extern void cpu_test(void);

int main(void)
{
    parser_test();
    cpu_test();
    ut_result();
    return 0;
}
