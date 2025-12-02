#include "flame.h"

void flame_init()
{
    pinMode(FLAME_DIGITAL_PIN, INPUT);                   // 设置火焰传感器数字引脚为输入
    analogReadResolution(12);                            // 12位（0~4095）
    analogSetPinAttenuation(FLAME_ANALOG_PIN, ADC_11db); // 约 0~3.3V 量程
}

int flame_read_digital()
{
    return digitalRead(FLAME_DIGITAL_PIN); // 读取火焰传感器数字值
}

int flame_read_analog()
{
    return analogRead(FLAME_ANALOG_PIN); // 读取火焰传感器模拟值
}   