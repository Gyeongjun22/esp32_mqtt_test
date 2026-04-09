#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"


#include "task_led.h"
#include "bsp_gpio.h"

#include "led.h"

// static const char* TAG = __FILE__;
static const char* TAG = "Task_led";

extern EventGroupHandle_t app_event_group;

static void task_led(void* arg)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        led_on(BSP_LED_RED);
        // led_off(BSP_LED_GREEN);
        vTaskDelayUntil(&xLastWakeTime, 100);

        led_off(BSP_LED_RED);
        // led_on(BSP_LED_GREEN);
        vTaskDelayUntil(&xLastWakeTime, 100);

        // EventBits_t bits =  xEventGroupWaitBits(
        //     app_event_group,
        //     EVT_BUTTON | EVT_TIMER,
        //     pdTRUE,
        //     pdFALSE,
        //     portMAX_DELAY
        // );

        // if (bits & EVT_BUTTON)
        // {
        //     led_on(BSP_LED_GREEN);
        //     led_off(BSP_LED_RED);
        //     ESP_LOGI(TAG, "Button Event Received!");
        // }

        // if (bits & EVT_TIMER)
        // {
        //     led_on(BSP_LED_RED);
        //     led_off(BSP_LED_GREEN);
        //     ESP_LOGI(TAG, "Timer Event Received!");
        // }

    }
}

TaskHandle_t xLedTaskHandle = NULL;
void task_led_init(void)
{
    ESP_LOGI(TAG, "Task led start");

    xTaskCreate(task_led, "Task_led", 2048, NULL, 11, &xLedTaskHandle);
    if(xLedTaskHandle == NULL){
        ESP_LOGE(TAG, "Task led test start fail");
    }
}