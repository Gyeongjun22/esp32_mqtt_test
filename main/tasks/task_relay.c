#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"


#include "led.h"
#include "relay.h"
#include "button.h"
#include "bsp_gpio.h"

#include "esp_event.h"
#include "task_event.h"

// static const char* TAG = __FILE__;
static const char* TAG = "Task_relay";

extern EventGroupHandle_t app_event_group;

static void task_relay(void* arg)
{
    for(;;)
    {
        EventBits_t bits = xEventGroupWaitBits(
            app_event_group,
            EVT_BUTTON | EVT_TIMER | EVT_RELAY_ON | EVT_RELAY_OFF,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );

        if (bits & EVT_BUTTON)
        {
            relay_on();
        }
        if (bits & EVT_TIMER)
        {
            relay_off();
        }
        if (bits & EVT_RELAY_ON)
        {
            ESP_LOGI(TAG, "MQTT: relay on");
            relay_on();
        }
        if (bits & EVT_RELAY_OFF)
        {
            ESP_LOGI(TAG, "MQTT: relay off");
            relay_off();
        }
    }
}

void task_relay_init(void)
{
    TaskHandle_t xHandle = NULL;
    ESP_LOGI(TAG, "start");

    xTaskCreate(task_relay, "Task_relay", 2048, NULL, 11, &xHandle);
    if(xHandle == NULL){
        ESP_LOGE(TAG, "task_relay start fail");
    }
}