#ifndef __FLAME_H_
#define __FLAME_H_

#include <Arduino.h>
#define FLAME_DIGITAL_PIN 16 // 火焰传感器数字引脚
#define FLAME_ANALOG_PIN 35  // 火焰传感器模拟引脚

void flame_init();
int flame_read_digital();
int flame_read_analog();

#endif