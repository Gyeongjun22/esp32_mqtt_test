/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "bsp_gpio.h"
#include "task_led.h"
#include "task_relay.h"
#include "task_button.h"
#include "task_hwtimer.h"
#include "task_ble.h"
#include "task_event.h"
#include "task_wifi.h"
#include "task_mqtt.h"

#include "swtimer.h"
#include "console.h"
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{
    initialize_nvs();
    bsp_gpio_init();

    task_event_init();  // 이벤트 그룹 생성 & task_event 실행

    task_wifi_init();   // WiFi 연결 (IP 획득 대기)
    task_mqtt_init();   // MQTT v5 클라이언트 시작

    task_ble_main();
    
    task_button_init();
    task_hwtimer_init();

    task_led_init();
    task_relay_init();
    sw_timer_init();

    console_init();

}
