#include "light_bee.h"

uint8_t light_state = 0; // 0: off, 1: on
uint8_t bee_state = 0;   // 0: off, 1: on

void light_bee_init()
{
    // 初始化蜂鸣器引脚（假设使用 GPIO 13，根据实际接线修改）
    pinMode(BEE_PIN, OUTPUT);
    digitalWrite(BEE_PIN, BEE_ON); // 初始关闭蜂鸣器

    // 初始化灯光引脚（假设使用 GPIO 4，根据实际接线修改）
    pinMode(LIGHT_PIN, OUTPUT);
    digitalWrite(LIGHT_PIN, LIGHT_ON); // 初始关闭灯光

    delay(1000);

    ALIGHT_OFF();
    ABEE_OFF();
}

void ABEE_ON()
{
    digitalWrite(BEE_PIN, BEE_ON);
}

void ABEE_OFF()
{
    digitalWrite(BEE_PIN, BEE_OFF);
}

void ALIGHT_ON()
{
    digitalWrite(LIGHT_PIN, LIGHT_ON);
}
void ALIGHT_OFF()
{
    digitalWrite(LIGHT_PIN, LIGHT_OFF);
}

void light_control(void)
{
    if (light_state == 0)
    {
        ALIGHT_OFF();
    }
    else
    {
        ALIGHT_ON();
    }
}

void bee_control(void)
{
    if (bee_state == 0)
    {
        ABEE_OFF();
    }
    else
    {
        ABEE_ON();
    }
}

uint8_t get_light_state(void)
{
    return light_state;
}

uint8_t get_bee_state(void)
{
    return bee_state;
}