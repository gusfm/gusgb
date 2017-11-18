#include <string.h>
#include "cartridge/mbc3.h"
#include "ut.h"

static int check_rtc_update(rtc_time_t *time, time_t diff,
                            rtc_time_t *rtc_expected)
{
    mbc3_rtc_update(time, diff);
    printf("+%.8u: ", (unsigned int)diff);
    rtc_print(time);
    ASSERT(time->sec == rtc_expected->sec);
    ASSERT(time->min == rtc_expected->min);
    ASSERT(time->hour == rtc_expected->hour);
    ASSERT(time->dayl == rtc_expected->dayl);
    ASSERT(time->dayh == rtc_expected->dayh);
    return 0;
}

static int rtc_update(void)
{
    rtc_time_t time, rtc_expected;
    memset(&time, 0, sizeof(time));
    memset(&rtc_expected, 0, sizeof(rtc_expected));
    ASSERT(check_rtc_update(&time, 0, &rtc_expected) == 0);
    rtc_expected.sec = 1;
    ASSERT(check_rtc_update(&time, 1, &rtc_expected) == 0);
    rtc_expected.sec = 59;
    ASSERT(check_rtc_update(&time, 58, &rtc_expected) == 0);
    rtc_expected.sec = 0;
    rtc_expected.min = 1;
    ASSERT(check_rtc_update(&time, 1, &rtc_expected) == 0);
    rtc_expected.min = 2;
    ASSERT(check_rtc_update(&time, 60, &rtc_expected) == 0);
    rtc_expected.min = 59;
    ASSERT(check_rtc_update(&time, 3420, &rtc_expected) == 0);
    rtc_expected.min = 0;
    rtc_expected.hour = 1;
    ASSERT(check_rtc_update(&time, 60, &rtc_expected) == 0);
    rtc_expected.hour = 2;
    ASSERT(check_rtc_update(&time, 3600, &rtc_expected) == 0);
    rtc_expected.hour = 23;
    ASSERT(check_rtc_update(&time, 75600, &rtc_expected) == 0);
    rtc_expected.sec = 59;
    rtc_expected.min = 59;
    ASSERT(check_rtc_update(&time, 3599, &rtc_expected) == 0);
    rtc_expected.sec = 0;
    rtc_expected.min = 0;
    rtc_expected.hour = 0;
    rtc_expected.dayl = 1;
    ASSERT(check_rtc_update(&time, 1, &rtc_expected) == 0);
    rtc_expected.dayl = 2;
    ASSERT(check_rtc_update(&time, 86400, &rtc_expected) == 0);
    rtc_expected.dayl = 0xff;
    rtc_expected.dayh = 0x1;
    ASSERT(check_rtc_update(&time, 86400 * 509, &rtc_expected) == 0);
    rtc_expected.dayl = 0;
    rtc_expected.dayh = 0x80;
    ASSERT(check_rtc_update(&time, 86400, &rtc_expected) == 0);
    rtc_expected.sec = 1;
    rtc_expected.dayh = 0;
    ASSERT(check_rtc_update(&time, 1, &rtc_expected) == 0);
    return 0;
}

void mbc3_test(void);

void mbc3_test(void)
{
    ut_run(rtc_update);
}
