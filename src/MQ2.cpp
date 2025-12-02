#include <Arduino.h>
#include <MQUnifiedsensor.h>

// 硬件参数与传感器配置
#define BOARD "ESP32"
#define VOLTAGE_RESOLUTION 3.3 // ESP32 ADC 参考电压（默认 3.3V）
#define ADC_BIT_RESOLUTION 12  // ESP32 默认 12 位 ADC
#define PIN_MQ2 34             // 建议用 GPIO34/35/36/39（仅输入）。根据你的接线修改此值。
#define MQ_TYPE "MQ-2"

// MQUnifiedsensor 实例
MQUnifiedsensor MQ2(BOARD, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, PIN_MQ2, MQ_TYPE);

// 清洁空气常数（Clean Air Factor）——来自官方示例/数据手册
// MQ-2 在清洁空气的 Rs/R0 典型值约为 9.83
static constexpr float MQ2_CLEAN_AIR_RATIO = 9.83f;

// 读取不同气体时使用的指数拟合曲线参数（A、B），来自 MQUnifiedsensor 示例
// 注意：这些系数用于估算 ppm，仅供相对/趋势参考，具体以你的器件与标定为准。
struct MQCurve
{
    float A;
    float B;
};
static constexpr MQCurve CURVE_LPG = {574.25f, -2.222f};
static constexpr MQCurve CURVE_CO = {36974.f, -3.109f};
static constexpr MQCurve CURVE_SMOKE = {958.7f, -2.476f};

static bool g_mq2_valid = false;

// 在清洁空气中校准 MQ-2 传感器的 R0
void calibrateMQ2()
{
    Serial.println("calibrating...");
    // 确保 ADC 配置适配 0~3.3V 量程
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_MQ2, ADC_11db);

    // 先读几次原始值做诊断
    {
        const int N = 8;
        long sum = 0;
        for (int i = 0; i < N; ++i)
        {
            sum += analogRead(PIN_MQ2);
            delay(2);
        }
        int avg = sum / N;
        float v = avg * VOLTAGE_RESOLUTION / ((1 << ADC_BIT_RESOLUTION) - 1);
        Serial.printf("[MQ2] ADC avg=%d (~%.3fV) before calibration\r\n", avg, v);
        if (avg <= 5)
            Serial.println("[MQ2] 提示：读数接近0，检查AO接线/供电/分压。");
        if (avg >= 4090)
            Serial.println("[MQ2] 提示：读数接近满量程，检查是否>3.3V或分压不足。");
    }
    MQ2.setRegressionMethod(1); // 1 = 指数回归，0 = 线性
    MQ2.init();

    float r0_sum = 0.0f;
    const int samples = 10;
    for (int i = 0; i < samples; ++i)
    {
        MQ2.update();
        r0_sum += MQ2.calibrate(MQ2_CLEAN_AIR_RATIO);
        delay(500);
    }
    const float r0 = r0_sum / samples;
    MQ2.setR0(r0);

    if (isnan(r0) || isinf(r0) || r0 <= 0.0f)
    {
        Serial.println("calibration failed: invalid R0, please check wiring and power!");
        g_mq2_valid = false;
        return;
    }

    g_mq2_valid = true;
    Serial.print("calibration completed, R0=");
    Serial.println(r0, 3);
}

// 读取指定气体的估算浓度（ppm）
float readGas(const MQCurve &curve)
{
    MQ2.setA(curve.A);
    MQ2.setB(curve.B);
    return MQ2.readSensor(); // ppm
}

void MQ2_update()
{
    MQ2.update();
}
float MQ2_getData()
{
    if (!g_mq2_valid)
        return NAN;
    return readGas(CURVE_SMOKE);
}