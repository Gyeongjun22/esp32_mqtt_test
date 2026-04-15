#include "task_fota.h"
#include "task_mqtt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

#include <string.h>

static const char *TAG = "task_fota";

#define FOTA_TASK_STACK  8192
#define FOTA_TASK_PRI    5
#define FOTA_URL_MAX     256

static char s_fota_url[FOTA_URL_MAX];

static void fota_task(void *arg)
{
    ESP_LOGI(TAG, "OTA 시작: %s", s_fota_url);
    mqtt_publish("esp32/fota/status", "started");

    esp_http_client_config_t http_cfg = {
        .url               = s_fota_url,
        .timeout_ms        = 15000,
        .keep_alive_enable = true,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_https_ota_config_t ota_cfg = {
        .http_config = &http_cfg,
    };

    esp_err_t ret = esp_https_ota(&ota_cfg);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA 성공 — 1초 후 재부팅");
        mqtt_publish("esp32/fota/status", "success");
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_restart();
    } else {
        ESP_LOGE(TAG, "OTA 실패: %s", esp_err_to_name(ret));
        mqtt_publish("esp32/fota/status", "failed");
    }

    vTaskDelete(NULL);
}

void task_fota_start(const char *url)
{
    strncpy(s_fota_url, url, FOTA_URL_MAX - 1);
    s_fota_url[FOTA_URL_MAX - 1] = '\0';

    xTaskCreate(fota_task, "fota_task", FOTA_TASK_STACK, NULL, FOTA_TASK_PRI, NULL);
}

void task_fota_init(void)
{
    ESP_LOGI(TAG, "FOTA 준비 완료");
}
