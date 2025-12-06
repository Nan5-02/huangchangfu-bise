#include "rtc.h"

static bool s_rtc_synced = false;

void rtc_begin(const char *ntpServer, long gmtOffsetSec, int daylightOffsetSec)
{
    // 配置 NTP（Arduino 封装的 IDF SNTP）
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);

    // 最多等待 10 次尝试（约 10 秒）
    for (int i = 0; i < 10; ++i)
    {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 1000))
        {
            s_rtc_synced = true;
            Serial.printf("[RTC] synced: %04d-%02d-%02d %02d:%02d:%02d\r\n",
                          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            return;
        }
        delay(500);
    }
    Serial.println("[RTC] sync failed (NTP timeout)");
    s_rtc_synced = false;
}

bool rtc_get_local(struct tm &out)
{
    if (getLocalTime(&out, 10))
    {
        s_rtc_synced = true;
        return true;
    }
    return false;
}

String rtc_now_string()
{
    struct tm t;
    if (!rtc_get_local(t))
        return String("");
    char buf[24];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return String(buf);
}

void rtc_read_local(uint8_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute)
{
    struct tm out;
    rtc_get_local(out);
    month = out.tm_mon + 1;
    day = out.tm_mday;
    hour = out.tm_hour;
    minute = out.tm_min;
}

bool rtc_is_synced()
{
    return s_rtc_synced;
}
