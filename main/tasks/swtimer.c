#include "swtimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/projdefs.h"


#include "task_led.h"
#include "bsp_gpio.h"

#include "led.h"
#include "relay.h"
#include "button.h"

#include "esp_log.h"

#define RELAY_OFF_TIME  10000   // 10sec

static TimerHandle_t relay_off_timer;
// static const char* TAG = __FILE__;
static const char* TAG = "SW_Timer";

void sw_timer_callback(TimerHandle_t xTimer)
{
    relay_off();
    ESP_LOGI(TAG, "relay off timer");
}


void sw_timer_init(void)
{
    ESP_LOGI(TAG, "sw_timer_init");
    
    relay_off_timer = xTimerCreate(
        "relay_off_timer", 
        pdMS_TO_TICKS(RELAY_OFF_TIME),
        pdTRUE, 
        NULL, 
        sw_timer_callback
    );
    
    xTimerStart(relay_off_timer, 0);
}
