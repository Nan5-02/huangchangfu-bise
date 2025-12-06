#ifndef __W25Q64_H_
#define __W25Q64_H_

#include <Arduino.h>

void w25q64_init();
void w25q64_periodic();
// 将时间戳写入对应地址槽位（按分钟映射），若槽位已占用则返回 false
bool w25q64_write(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t *arry, size_t len);

bool w25q64_read(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t *arry, size_t len);

#endif // !__W25Q64_DEMO_H_
