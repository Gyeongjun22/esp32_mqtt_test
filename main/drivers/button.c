#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_timer.h"

#include "driver/gpio.h"
#include "bsp_gpio.h"

//#include "rtc.h"

#include "button.h"

QueueHandle_t button_evt_queue;

static void IRAM_ATTR button_isr_handler(void *arg)
{
    if (button_evt_queue == NULL) return;

    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(button_evt_queue, &gpio_num, NULL);
}



void button_init(void)
{
    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    assert(button_evt_queue);

    gpio_isr_handler_add(BSP_MCU_BUTTON, button_isr_handler, (void *)BSP_MCU_BUTTON);

}


