#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "net_config.h"
#include "cwifi.h"

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);
static unsigned long lastMqttReconnectAttempt = 0;

static String makeClientId()
{
    uint64_t mac = ESP.getEfuseMac();
    char buf[32];
    snprintf(buf, sizeof(buf), "%s-%04X", MQTT_CLIENT_ID_PREFIX, (uint16_t)(mac & 0xFFFF));
    return String(buf);
}

static void onMqttMessage(char *topic, byte *payload, unsigned int len)
{
    Serial.print("[MQTT] <- ");
    Serial.print(topic);
    Serial.print(" | ");
    for (unsigned int i = 0; i < len; i++)
        Serial.print((char)payload[i]);
    Serial.println();
}

static bool mqttConnect()
{
    String cid = makeClientId();
    Serial.printf("[MQTT] Connecting to %s:%d as %s...\r\n", MQTT_HOST, MQTT_PORT, cid.c_str());
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    mqtt.setCallback(onMqttMessage);

    bool ok;
    if (strlen(MQTT_USER) > 0)
        ok = mqtt.connect(cid.c_str(), MQTT_USER, MQTT_PASS);
    else
        ok = mqtt.connect(cid.c_str());

    if (ok)
    {
        Serial.println("[MQTT] Connected.");
        if (strlen(MQTT_TOPIC_SUB) > 0)
        {
            mqtt.subscribe(MQTT_TOPIC_SUB);
            Serial.printf("[MQTT] Subscribed: %s\r\n", MQTT_TOPIC_SUB);
        }
    }
    else
    {
        Serial.printf("[MQTT] Failed, rc=%d\r\n", mqtt.state());
    }
    return ok;
}

void net_begin()
{
    // WiFi
    Serial.printf("[WiFi] Connecting to %s...\r\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    //等待连接WIFI
    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000)
    {
        delay(300);
        Serial.print('.');
    }
    Serial.println();

    // 输出连接结果
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.printf("[WiFi] Connected: %s | IP: %s\r\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
    else
    {
        Serial.println("[WiFi] Connect timeout. Will keep retrying in loop().");
    }

    // MQTT 首次尝试连接（若 WiFi 已连）
    if (WiFi.status() == WL_CONNECTED)
    {
        mqttConnect();
    }
}

void net_loop()
{
    // 保持 WiFi 连接
    if (WiFi.status() != WL_CONNECTED)
    {
        static unsigned long lastWiFiTry = 0;
        if (millis() - lastWiFiTry > 5000)
        {
            lastWiFiTry = millis();
            Serial.println("[WiFi] Reconnecting...");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        }
        return; // WiFi 未连，先不跑 MQTT
    }

    // 保持 MQTT 连接
    if (!mqtt.connected())
    {
        unsigned long now = millis();
        if (now - lastMqttReconnectAttempt > 3000)
        {
            lastMqttReconnectAttempt = now;
            mqttConnect();
        }
        return;
    }

    mqtt.loop();
}

bool net_publish(const char *topic, const char *payload)
{
    if (!mqtt.connected())
        return false;
    return mqtt.publish(topic, payload);
}
