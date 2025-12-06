#include <Arduino.h>
#include <WiFi.h>
#include "MQ2.h"
#include "w25q64.h"
#include "light_bee.h"
#include "flame.h"
#include "cwifi.h"
#include "net_config.h"
#include "rtc.h"
#include "tim.h"

float smoke;
int flame_digital;

void setup()
{
  // 初始化串口
  Serial.begin(115200);
  // 初始化灯和蜂鸣器
  light_bee_init();
  // 校准 MQ-2 传感器
  calibrateMQ2();
  // 初始化外部 SPI Flash W25Q64
  w25q64_init();
  // 初始化火焰传感器
  flame_init();
  // 连接 WiFi 与 MQTT（参数在 include/net_config.h 中配置）
  net_begin();
  // 初始化 RTC（NTP 同步）
  rtc_begin("ntp.aliyun.com", 8 * 3600, 0);
  // 启动定时器
  tim_begin(1000000); // 1 秒中断
}

void loop()
{

  if (get_second_flag())
  {
    // 建议每次读取前调用 update()
    MQ2_update();
    smoke = MQ2_getData();
    // 读取火焰传感器数字值
    flame_digital = flame_read_digital();

    struct tm timeinfo;
    rtc_get_local(timeinfo);
    uint8_t hour = timeinfo.tm_hour;
    uint8_t minute = timeinfo.tm_min;
    uint8_t second = timeinfo.tm_sec;

    // 发布数据到MQTT
    char time[128];
    sprintf(time, "%d:%d:%d\r\n", hour, minute, second);
    net_publish("esp32/smoke", String(smoke).c_str());
    net_publish("esp32/flame", String(flame_digital).c_str());
    net_publish("esp32/time", time);

    // 维持 MQTT 心跳与重连
    net_loop();
    clear_second_flag();
  }

  if (get_minute_flag())
  {
    uint8_t month;
    uint16_t day, hour, minute;
    uint8_t data[4] = {0};
    uint8_t buffer[8] = {0};
    rtc_read_local(month, day, hour, minute);
    printf("Current Time: %02d-%02d %02d:%02d\r\n", month, day, hour, minute);

    memcpy(data, &smoke, sizeof(smoke)); // 将 float 数据拷贝到字节数组
    memcpy(buffer, data, sizeof(data));  // 紧接着拷贝 float 数据
    buffer[4] = flame_digital;
    buffer[5] = get_light_state(); // 预留位
    buffer[6] = get_bee_state();   // 预留位
    buffer[7] = 0xFF;              // 预留位

    // 写入 SPI Flash
    bool ok = w25q64_write(month, day, hour, minute, buffer, sizeof(buffer));
    if (ok)
    {
      Serial.println("W25Q64 write success.");
    }
    else
    {
      Serial.println("W25Q64 write failed.");
    }

    net_publish("esp32/light", String(get_light_state()).c_str());
    net_publish("esp32/bee", String(get_bee_state()).c_str());

    clear_minute_flag();
  }
}