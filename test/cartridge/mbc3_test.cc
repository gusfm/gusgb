#include <gtest/gtest.h>
extern "C" {
#include "cartridge/mbc3.h"
}

static void rtc_print(rtc_t *rtc)
{
    unsigned int day = (unsigned int)rtc->dayl + ((rtc->dayh & 1) << 8);
    printf("%.2hhu:%.2hhu:%.2hhu %u\n", rtc->hour, rtc->min, rtc->sec,
           day);
}

static void test_rtc_update(rtc_t *rtc, time_t diff, rtc_t *rtc_expected)
{
    mbc3_rtc_update(rtc, diff);
    printf("+%.8u: ", (unsigned int)diff);
    rtc_print(rtc);
    EXPECT_EQ(rtc->sec, rtc_expected->sec);
    EXPECT_EQ(rtc->min, rtc_expected->min);
    EXPECT_EQ(rtc->hour, rtc_expected->hour);
    EXPECT_EQ(rtc->dayl, rtc_expected->dayl);
    EXPECT_EQ(rtc->dayh, rtc_expected->dayh);
}

TEST(mbc3_test, rtc_update)
{
    rtc_t rtc, rtc_expected;
    memset(&rtc, 0, sizeof(rtc));
    memset(&rtc_expected, 0, sizeof(rtc_expected));
    test_rtc_update(&rtc, 0, &rtc_expected);
    rtc_expected.sec = 1;
    test_rtc_update(&rtc, 1, &rtc_expected);
    rtc_expected.sec = 59;
    test_rtc_update(&rtc, 58, &rtc_expected);
    rtc_expected.sec = 0;
    rtc_expected.min = 1;
    test_rtc_update(&rtc, 1, &rtc_expected);
    rtc_expected.min = 2;
    test_rtc_update(&rtc, 60, &rtc_expected);
    rtc_expected.min = 59;
    test_rtc_update(&rtc, 3420, &rtc_expected);
    rtc_expected.min = 0;
    rtc_expected.hour = 1;
    test_rtc_update(&rtc, 60, &rtc_expected);
    rtc_expected.hour = 2;
    test_rtc_update(&rtc, 3600, &rtc_expected);
    rtc_expected.hour = 23;
    test_rtc_update(&rtc, 75600, &rtc_expected);
    rtc_expected.sec = 59;
    rtc_expected.min = 59;
    test_rtc_update(&rtc, 3599, &rtc_expected);
    rtc_expected.sec = 0;
    rtc_expected.min = 0;
    rtc_expected.hour = 0;
    rtc_expected.dayl = 1;
    test_rtc_update(&rtc, 1, &rtc_expected);
    rtc_expected.dayl = 2;
    test_rtc_update(&rtc, 86400, &rtc_expected);
    rtc_expected.dayl = 0xff;
    rtc_expected.dayh = 0x1;
    test_rtc_update(&rtc, 86400 * 509, &rtc_expected);
    rtc_expected.dayl = 0;
    rtc_expected.dayh = 0x80;
    test_rtc_update(&rtc, 86400, &rtc_expected);
    rtc_expected.sec = 1;
    rtc_expected.dayh = 0;
    test_rtc_update(&rtc, 1, &rtc_expected);
}
