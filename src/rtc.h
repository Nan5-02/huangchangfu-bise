#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <time.h>

// 初始化 RTC：配置 NTP，等待一次同步。默认使用 pool.ntp.org，UTC+8。
void rtc_begin(const char *ntpServer = "pool.ntp.org", long gmtOffsetSec = 8 * 3600, int daylightOffsetSec = 0);

// 获取当前本地时间（已应用时区偏移）。成功返回 true。
bool rtc_get_local(struct tm &out);

// 获取格式化时间字符串，例如 "2025-12-06 14:03:15"。失败返回空串。
String rtc_now_string();

// 判断是否已同步（通过 getLocalTime 成功与否）
bool rtc_is_synced();

// 读取本地时间的月、日、时、分
void rtc_read_local(uint8_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute);

#endif