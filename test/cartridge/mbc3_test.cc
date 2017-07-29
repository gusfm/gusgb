#include <gtest/gtest.h>
extern "C" {
#include "cartridge/mbc3.h"
}

static void test_rtc_update(rtc_time_t *time, time_t diff,
                            rtc_time_t *rtc_expected)
{
    mbc3_rtc_update(time, diff);
    printf("+%.8u: ", (unsigned int)diff);
    rtc_print(time);
    EXPECT_EQ(time->sec, rtc_expected->sec);
    EXPECT_EQ(time->min, rtc_expected->min);
    EXPECT_EQ(time->hour, rtc_expected->hour);
    EXPECT_EQ(time->dayl, rtc_expected->dayl);
    EXPECT_EQ(time->dayh, rtc_expected->dayh);
}

TEST(mbc3_test, rtc_update)
{
    rtc_time_t time, rtc_expected;
    memset(&time, 0, sizeof(time));
    memset(&rtc_expected, 0, sizeof(rtc_expected));
    test_rtc_update(&time, 0, &rtc_expected);
    rtc_expected.sec = 1;
    test_rtc_update(&time, 1, &rtc_expected);
    rtc_expected.sec = 59;
    test_rtc_update(&time, 58, &rtc_expected);
    rtc_expected.sec = 0;
    rtc_expected.min = 1;
    test_rtc_update(&time, 1, &rtc_expected);
    rtc_expected.min = 2;
    test_rtc_update(&time, 60, &rtc_expected);
    rtc_expected.min = 59;
    test_rtc_update(&time, 3420, &rtc_expected);
    rtc_expected.min = 0;
    rtc_expected.hour = 1;
    test_rtc_update(&time, 60, &rtc_expected);
    rtc_expected.hour = 2;
    test_rtc_update(&time, 3600, &rtc_expected);
    rtc_expected.hour = 23;
    test_rtc_update(&time, 75600, &rtc_expected);
    rtc_expected.sec = 59;
    rtc_expected.min = 59;
    test_rtc_update(&time, 3599, &rtc_expected);
    rtc_expected.sec = 0;
    rtc_expected.min = 0;
    rtc_expected.hour = 0;
    rtc_expected.dayl = 1;
    test_rtc_update(&time, 1, &rtc_expected);
    rtc_expected.dayl = 2;
    test_rtc_update(&time, 86400, &rtc_expected);
    rtc_expected.dayl = 0xff;
    rtc_expected.dayh = 0x1;
    test_rtc_update(&time, 86400 * 509, &rtc_expected);
    rtc_expected.dayl = 0;
    rtc_expected.dayh = 0x80;
    test_rtc_update(&time, 86400, &rtc_expected);
    rtc_expected.sec = 1;
    rtc_expected.dayh = 0;
    test_rtc_update(&time, 1, &rtc_expected);
}
