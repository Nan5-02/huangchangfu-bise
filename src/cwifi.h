#ifndef __CWIFI_H_
#define __CWIFI_H_

#include <Arduino.h>

void net_begin();                                         // 连接 WiFi 并初始化 MQTT
void net_loop();                                          // 放在 loop() 中维持 MQTT 心跳与重连
bool net_publish(const char *topic, const char *payload); // 简易发布接口

#endif