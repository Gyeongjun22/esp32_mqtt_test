#include "driver/gptimer.h"
#include "hwtimer.h"
#include "esp_log.h"
#include "esp_err.h"

#include "esp_attr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "bsp_gpio.h"

static const char *TAG = "HW_TIMER";

QueueHandle_t timer_evt_queue;


// ISR Callback func (인터럽트 발생 시 실행)
static bool IRAM_ATTR example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    if (timer_evt_queue == NULL) {
        return false;
    }
    uint32_t gpio_num = (uint32_t)BSP_LED_RED;
    xQueueSendFromISR(timer_evt_queue, &gpio_num, NULL);
    
    return false;
}

void hwtimer_init(uint32_t interval_ms)
{
    timer_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    assert(timer_evt_queue);



    static gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = interval_ms * 1000,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
