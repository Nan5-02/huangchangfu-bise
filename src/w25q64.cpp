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
static const uint32_t LOG_BASE_ADDR = 0x001000;     // 日志起始地址（留出前面演示区）
static const uint32_t LOG_RECORD_SIZE = 8;          // 每条记录 8 字节

struct __attribute__((packed)) LogRecord
{
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t rsv1;
    uint8_t rsv2;
    uint8_t rsv3;
};

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

// 基于时间计算地址并写入一条 8 字节记录
// 地址映射：index = ((month-1)*31 + (day-1)) * 1440 + hour*60 + minute
// 这样以“分钟”为粒度，覆盖全年（最多 372 天*1440 分钟 ≈ 535k 条），每条 8 字节，占用约 4.2MB，适配 8MB 容量。
bool w25q64_write(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t *arry, size_t len)
{
    if (!flashReady)
    {
        Serial.println("[W25Q64] not ready");
        return false;
    }

    // 合法性检查（简单范围防护）
    if (month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59)
    {
        Serial.println("[W25Q64] invalid time params");
        return false;
    }

    // 以分钟为粒度映射到索引
    uint32_t index = ((uint32_t)(month - 1) * 31 + (uint32_t)(day - 1)) * 1440u + (uint32_t)hour * 60u + (uint32_t)minute;

    uint64_t addr = LOG_BASE_ADDR + (uint64_t)index * LOG_RECORD_SIZE;

    // 容量检查
    uint64_t cap = flash.getCapacity();
    if (addr + LOG_RECORD_SIZE > cap)
    {
        Serial.println("[W25Q64] address out of range");
        return false;
    }

    // 检查槽位是否已被写过（避免整扇区擦除破坏其他数据）
    uint8_t probe[LOG_RECORD_SIZE];
    flash.readByteArray(addr, probe, LOG_RECORD_SIZE);
    bool freeSlot = true;
    for (uint8_t b : probe)
    {
        if (b != 0xFF)
        {
            freeSlot = false;
            break;
        }
    }
    if (!freeSlot)
    {
        Serial.println("[W25Q64] slot already used; skip writing to avoid erasing other data.");
        return false;
    }

    bool ok = flash.writeByteArray(addr, arry, len);
    if (!ok)
    {
        Serial.println("[W25Q64] write failed");
    }
    return ok;
}

// 基于时间计算地址并读取一条 8 字节记录
// 地址映射：index = ((month-1)*31 + (day-1)) * 1440 + hour*60 + minute
bool w25q64_read(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t *arry, size_t len)
{
    if (!flashReady)
    {
        Serial.println("[W25Q64] not ready");
        return false;
    }

    // 合法性检查（简单范围防护）
    if (month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59)
    {
        Serial.println("[W25Q64] invalid time params");
        return false;
    }

    // 以分钟为粒度映射到索引
    uint32_t index = ((uint32_t)(month - 1) * 31 + (uint32_t)(day - 1)) * 1440u + (uint32_t)hour * 60u + (uint32_t)minute;

    uint64_t addr = LOG_BASE_ADDR + (uint64_t)index * LOG_RECORD_SIZE;

    // 容量检查
    uint64_t cap = flash.getCapacity();
    if (addr + LOG_RECORD_SIZE > cap)
    {
        Serial.println("[W25Q64] address out of range");
        return false;
    }

    bool ok = flash.readByteArray(addr, arry, len);
    if (!ok)
    {
        Serial.println("[W25Q64] read failed");
    }
    return ok;
}
