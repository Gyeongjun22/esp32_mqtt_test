#include "swtimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/projdefs.h"

#include "esp_log.h"

#include "task_led.h"
#include "bsp_gpio.h"

#include "led.h"
#include "relay.h"
#include "button.h"

#define TIME_10s  10000   // 10sec

static TimerHandle_t timer_10s;
static const char* TAG = "SW_Timer";

void sw_timer_callback(TimerHandle_t xTimer)
{
    ESP_LOGI(TAG, "relay off timer");
}


void sw_timer_init(void)
{
    ESP_LOGI(TAG, "sw_timer_init");
    
    timer_10s = xTimerCreate(
        "relay_off_timer", 
        pdMS_TO_TICKS(TIME_10s),
        pdTRUE, 
        NULL, 
        sw_timer_callback
    );
    
    xTimerStart(timer_10s, 0);
}

void sw_timer_stop(void)
{
    if (timer_10s == NULL) return;

    ESP_LOGI(TAG, "SW timer stop");
    xTimerStop(timer_10s, 0);
    timer_10s = NULL;
}
