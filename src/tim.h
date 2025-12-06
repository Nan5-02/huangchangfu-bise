#ifndef __TIM_H
#define __TIM_H

#include <Arduino.h>

void tim_begin(uint64_t interval_us);
uint8_t get_second_flag();
uint8_t get_minute_flag();
void clear_second_flag();
void clear_minute_flag();

#endif
