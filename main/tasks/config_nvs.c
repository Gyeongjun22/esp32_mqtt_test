#include "config_nvs.h"

#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define NVS_NAMESPACE   "config"
#define KEY_WIFI_SSID   "wifi_ssid"
#define KEY_WIFI_PASS   "wifi_pass"
#define KEY_MQTT_URI    "mqtt_uri"
#define KEY_MQTT_PORT   "mqtt_port"
#define KEY_MQTT_CID    "mqtt_cid"

static const char *TAG = "config_nvs";

// ── 내부 헬퍼 ─────────────────────────────────────────────────

static void nvs_set_str_safe(const char *key, const char *value)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "nvs_open failed: %s", esp_err_to_name(err));
        return;
    }
    nvs_set_str(h, key, value);
    nvs_commit(h);
    nvs_close(h);
}

static void nvs_get_str_safe(const char *key, char *buf, size_t len,
                              const char *fallback)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (err != ESP_OK) {
        strlcpy(buf, fallback, len);
        return;
    }
    size_t out_len = len;
    err = nvs_get_str(h, key, buf, &out_len);
    nvs_close(h);
    if (err != ESP_OK) {
        strlcpy(buf, fallback, len);
    }
}

// ── WiFi ──────────────────────────────────────────────────────

void config_nvs_set_wifi_ssid(const char *ssid)
{
    nvs_set_str_safe(KEY_WIFI_SSID, ssid);
    ESP_LOGI(TAG, "WiFi SSID 저장: %s", ssid);
}

void config_nvs_set_wifi_pass(const char *pass)
{
    nvs_set_str_safe(KEY_WIFI_PASS, pass);
    ESP_LOGI(TAG, "WiFi PW 저장 완료");
}

void config_nvs_get_wifi_ssid(char *buf, size_t len)
{
    nvs_get_str_safe(KEY_WIFI_SSID, buf, len, CFG_WIFI_SSID_DEFAULT);
}

void config_nvs_get_wifi_pass(char *buf, size_t len)
{
    nvs_get_str_safe(KEY_WIFI_PASS, buf, len, CFG_WIFI_PASS_DEFAULT);
}

// ── MQTT ──────────────────────────────────────────────────────

void config_nvs_set_mqtt_uri(const char *uri)
{
    nvs_set_str_safe(KEY_MQTT_URI, uri);
    ESP_LOGI(TAG, "MQTT URI 저장: %s", uri);
}

void config_nvs_set_mqtt_port(uint16_t port)
{
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) return;
    nvs_set_u16(h, KEY_MQTT_PORT, port);
    nvs_commit(h);
    nvs_close(h);
    ESP_LOGI(TAG, "MQTT port 저장: %u", port);
}

void config_nvs_set_mqtt_client_id(const char *id)
{
    nvs_set_str_safe(KEY_MQTT_CID, id);
    ESP_LOGI(TAG, "MQTT client_id 저장: %s", id);
}

void config_nvs_get_mqtt_uri(char *buf, size_t len)
{
    nvs_get_str_safe(KEY_MQTT_URI, buf, len, CFG_MQTT_URI_DEFAULT);
}

uint16_t config_nvs_get_mqtt_port(void)
{
    nvs_handle_t h;
    uint16_t port = CFG_MQTT_PORT_DEFAULT;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return port;
    nvs_get_u16(h, KEY_MQTT_PORT, &port);
    nvs_close(h);
    return port;
}

void config_nvs_get_mqtt_client_id(char *buf, size_t len)
{
    nvs_get_str_safe(KEY_MQTT_CID, buf, len, CFG_MQTT_CID_DEFAULT);
}
