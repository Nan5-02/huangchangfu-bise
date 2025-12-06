#pragma once

// ===== WiFi Credentials =====
#ifndef WIFI_SSID
#define WIFI_SSID "ace3pro"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "12345678"
#endif

// ===== MQTT Settings =====
#ifndef MQTT_HOST
#define MQTT_HOST "106.53.9.14" // 修改为你的 MQTT 服务器地址或域名
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883 // 如为 TLS，请使用 8883（本示例为非 TLS）
#endif
#ifndef MQTT_USER
#define MQTT_USER "" // 若无账号可留空
#endif
#ifndef MQTT_PASS
#define MQTT_PASS "" // 若无密码可留空
#endif
#ifndef MQTT_CLIENT_ID_PREFIX
#define MQTT_CLIENT_ID_PREFIX "esp32-node"
#endif
#ifndef MQTT_TOPIC_SUB
#define MQTT_TOPIC_SUB "esp32/in/#" // 订阅通配主题示例
#endif
#ifndef MQTT_TOPIC_PUB
#define MQTT_TOPIC_PUB "esp32/out" // 发布主题示例
#endif
