#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"


#include "led.h"
#include "relay.h"
#include "button.h"
#include "bsp_gpio.h"

#include "esp_event.h"
#include "task_event.h"
#include "task_mqtt.h"

static const char* TAG = "Task_button";
extern QueueHandle_t button_evt_queue;
extern EventGroupHandle_t app_event_group;


static void task_button(void* arg)
{
    uint32_t gpio_num;
    // TickType_t last_tick = 0;
    TickType_t press_tick = 0;
    TickType_t release_tick = 0;
    bool is_pressing = false;
    
    for(;;)
    {
        // 큐에서 버튼 이벤트를 대기
        if (xQueueReceive(button_evt_queue, &gpio_num, portMAX_DELAY)) 
        {
            int level = gpio_get_level(gpio_num);
            if (level == 0)
            {
                press_tick = xTaskGetTickCount();
                is_pressing = true;
            }
            else if (level == 1 && is_pressing)
            {
                release_tick = xTaskGetTickCount();
                is_pressing = false;

                TickType_t duration = release_tick - press_tick;

                // 1. 디바운싱 처리
                if (duration < pdMS_TO_TICKS(50)){
                    continue;
                }

                // 2. 시간 판별 (2초 이상이면 Long Press)
                if (duration >= pdMS_TO_TICKS(5000)){
                    ESP_LOGI(TAG, "EVT_BUTTON_LONG_LONG (%u ms)", (unsigned int)pdTICKS_TO_MS(duration));
                    xEventGroupSetBits(app_event_group, EVT_BUTTON_LONG_LONG);
                    mqtt_publish("esp32/button", "long_long");
                }
                else if (duration >= pdMS_TO_TICKS(2000)){
                    ESP_LOGI(TAG, "EVT_BUTTON_LONG (%u ms)", (unsigned int)pdTICKS_TO_MS(duration));
                    xEventGroupSetBits(app_event_group, EVT_BUTTON_LONG);
                    mqtt_publish("esp32/button", "long");
                }
                else{
                    ESP_LOGI(TAG, "EVT_BUTTON (Short) (%u ms)", (unsigned int)pdTICKS_TO_MS(duration));
                    xEventGroupSetBits(app_event_group, EVT_BUTTON);
                    mqtt_publish("esp32/button", "short");
                }
            }

            // TickType_t now = xTaskGetTickCount();
            // if (now - last_tick > pdMS_TO_TICKS(50)) {
            //     ESP_LOGI(TAG, "EVT_BUTTON");
            //     xEventGroupSetBits(app_event_group, EVT_BUTTON);
            //     last_tick = now;
            // }
        }
    }
}

void task_button_init(void)
{
    button_init();
    assert(app_event_group);
    TaskHandle_t xHandle = NULL;
    ESP_LOGI(TAG, "start");

    xTaskCreate(task_button, "Task_button", 2048, NULL, 11, &xHandle);
    if(xHandle == NULL){
        ESP_LOGE(TAG, "task_button start fail");
    }
}
