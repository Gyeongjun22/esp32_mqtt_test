#ifndef __CONFIG_NVS_H__
#define __CONFIG_NVS_H__

#include <stdint.h>
#include <stddef.h>

// ── 기본값 ────────────────────────────────────────────────────
#define CFG_WIFI_SSID_DEFAULT   ""
#define CFG_WIFI_PASS_DEFAULT   ""
#define CFG_AP_SSID_DEFAULT     "ESP32-AP"
#define CFG_AP_PASS_DEFAULT     "12345678"
#define CFG_MQTT_URI_DEFAULT    "mqtt://192.168.0.140"
#define CFG_MQTT_PORT_DEFAULT   1883
#define CFG_MQTT_CID_DEFAULT    "esp32-a02"

// ── WiFi ──────────────────────────────────────────────────────
void config_nvs_set_wifi_ssid(const char *ssid);
void config_nvs_set_wifi_pass(const char *pass);
void config_nvs_get_wifi_ssid(char *buf, size_t len);
void config_nvs_get_wifi_pass(char *buf, size_t len);

// ── MQTT ──────────────────────────────────────────────────────
void config_nvs_set_mqtt_uri(const char *uri);
void config_nvs_set_mqtt_port(uint16_t port);
void config_nvs_set_mqtt_client_id(const char *id);
void config_nvs_get_mqtt_uri(char *buf, size_t len);
uint16_t config_nvs_get_mqtt_port(void);
void config_nvs_get_mqtt_client_id(char *buf, size_t len);

#endif // __CONFIG_NVS_H__
