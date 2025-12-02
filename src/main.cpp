#include <Arduino.h>
#include "MQ2.h"
#include "w25q64.h"
#include "light_bee.h"
#include "flame.h"
#include "cwifi.h"
#include "net_config.h"

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
}

void loop()
{
  // 建议每次读取前调用 update()
  MQ2_update();
  float smoke = MQ2_getData();

  int flame_digital = flame_read_digital();
  int flame_analog = flame_read_analog();

  Serial.printf("Flame Digital: %d, Analog: %d, Smoke: %.2f ppm\r\n", flame_digital, flame_analog, smoke);

  // W25Q64 每 5 秒做一次计数读写演示
  w25q64_periodic();

  // 维持 MQTT 心跳与重连 
  net_loop();
  net_publish(MQTT_TOPIC_PUB, "Hello from ESP32");

  delay(1000);
}