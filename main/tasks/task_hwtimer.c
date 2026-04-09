#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_log.h"


#include "led.h"
#include "hwtimer.h"
#include "bsp_gpio.h"

#include "esp_event.h"
#include "task_event.h"


static const char* TAG = "Task_hwtimer";
extern QueueHandle_t timer_evt_queue;
extern EventGroupHandle_t app_event_group;

static void task_hwtimer(void* arg)
{
    uint32_t gpio_num;
    // TickType_t last_tick = 0;


    for(;;)
    {
        if (xQueueReceive(timer_evt_queue, &gpio_num, portMAX_DELAY)) {
            xEventGroupSetBits(app_event_group, EVT_TIMER);
        }
    }
}

void task_hwtimer_init(void)
{
    hwtimer_init(5000);
    assert(app_event_group);
    TaskHandle_t xHandle = NULL;
    ESP_LOGI(TAG, "start");

    xTaskCreate(task_hwtimer, "Task_timer", 2048, NULL, 11, &xHandle);
    if(xHandle == NULL){
        ESP_LOGE(TAG, "task_timer start fail");
    }
}
