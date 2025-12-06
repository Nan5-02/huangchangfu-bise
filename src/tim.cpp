#include <Arduino.h>

hw_timer_t *timer = nullptr;

uint8_t sencond_flag = 0;
uint8_t minute_flag = 0;
uint16_t count = 0;

void IRAM_ATTR onTimer()
{
    count++;

    sencond_flag = 1;

    if (count >= 60)
    {
        count = 0;
        minute_flag = 1;
    }
}

void tim_begin(uint64_t interval_us)
{
    // 1MHz 计数（APB 80MHz / 80），上升计数
    timer = timerBegin(0, 80, true);

    // 绑定一次中断回调（重复绑定会触发 timer_isr_callback_add 错误）
    timerAttachInterrupt(timer, &onTimer, true);

    // 设置报警值：传入的 interval_us 为微秒周期；auto-reload
    timerAlarmWrite(timer, interval_us, true);

    // 启动定时器
    timerAlarmEnable(timer);
}

uint8_t get_second_flag()
{
    return sencond_flag;
}

uint8_t get_minute_flag()
{
    return minute_flag;
}

void clear_second_flag()
{
    sencond_flag = 0;
}

void clear_minute_flag()
{
    minute_flag = 0;
}