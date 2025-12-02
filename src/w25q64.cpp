#include <SPI.h>
#include <SPIMemory.h>
#include "w25q64.h"

// 选择 W25Q64 的片选引脚（根据你的实际接线修改）。
#define FLASH_CS_PIN 5
#define FLASH_SCK_PIN 18
#define FLASH_MISO_PIN 19
#define FLASH_MOSI_PIN 23

// 创建 SPIFlash 对象（SPIMemory 库）
SPIFlash flash(FLASH_CS_PIN);

static uint32_t lastMillis = 0;

// 简单演示地址
static const uint32_t DEMO_ADDR_STRING = 0x000100;  // 存放测试字符串
static const uint32_t DEMO_ADDR_COUNTER = 0x000200; // 存放计数器

bool flashReady = false;

void w25q64_init()
{
    Serial.println("[W25Q64] init...");
    // 显式初始化 VSPI 引脚，并将 CS 拉高，避免上电时进入忙状态
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);
    SPI.begin(FLASH_SCK_PIN, FLASH_MISO_PIN, FLASH_MOSI_PIN, FLASH_CS_PIN);
    delay(10);

    if (!flash.begin())
    {
        Serial.println("[W25Q64] init failed");
        return;
    }

    flashReady = true;

    Serial.print("[W25Q64] Capacity(Bytes): ");
    Serial.println(flash.getCapacity() / 1024);

    // 写入一个测试字符串
    const char testData[] = "HelloFlash"; // 包含结尾 '\0'
    flash.eraseSector(DEMO_ADDR_STRING);  // 擦除所属扇区
    flash.writeByteArray(DEMO_ADDR_STRING, (uint8_t *)testData, sizeof(testData));

    char buffer[16] = {0};
    flash.readByteArray(DEMO_ADDR_STRING, (uint8_t *)buffer, sizeof(testData));
    Serial.print("[W25Q64] read back string: ");
    Serial.println(buffer);
}

void w25q64_periodic()
{
    if (!flashReady)
        return;
    if (millis() - lastMillis < 5000)
        return; // 每 5 秒执行一次
    lastMillis = millis();
}
