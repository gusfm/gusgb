#include "ut.h"

struct ut unit_test;

extern void mbc3_test(void);

int main(void)
{
    mbc3_test();
    ut_result();
    return 0;
}
