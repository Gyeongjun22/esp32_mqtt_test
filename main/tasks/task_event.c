#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"


#include "led.h"
#include "button.h"
#include "bsp_gpio.h"

#include "esp_event.h"
#include "task_led.h"
#include "task_event.h"

// static const char* TAG = __FILE__;
static const char* TAG = "Task_event";

EventGroupHandle_t app_event_group;
static char pcWriteBuffer[4096];

static void task_event(void* arg)
{
    for(;;)
    {
        EventBits_t bits = xEventGroupWaitBits(
            app_event_group,
            EVT_BUTTON | EVT_TIMER | EVT_BUTTON_LONG | EVT_BUTTON_LONG_LONG,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );
        
        if (bits & EVT_BUTTON)
        {
            led_on(BSP_LED_GREEN);
            led_off(BSP_LED_RED);
        }
        if (bits & EVT_TIMER)
        {
            led_off(BSP_LED_GREEN);
            led_off(BSP_LED_RED);
        }
        if (bits & EVT_BUTTON_LONG)
        {
            // xLedTaskHandle에 직접 접근하지 않고 함수로 제어
            ESP_LOGI(TAG, "버튼 입력 감지: LED Task 토글");
            task_led_toggle();
        }
        if (bits & EVT_BUTTON_LONG_LONG)
        {
            ESP_LOGI(TAG, "=== Task List ===");

            vTaskList(pcWriteBuffer);

            printf("Task Name\tState\tPrio\tStack\tNum\n");
            printf("------------------------------------------------\n");
            printf("%s\n", pcWriteBuffer);
        }
    }
}

EventGroupHandle_t task_event_get_handle(void)
{
    return app_event_group;
}

void task_event_init(void)
{
    TaskHandle_t xHandle = NULL;
    ESP_LOGI(TAG, "start");
    app_event_group = xEventGroupCreate();

    xTaskCreate(task_event, "Task_event", 2048, NULL, 10, &xHandle);
    if(xHandle == NULL){
        ESP_LOGE(TAG, "task_event start fail");
    }
}
