#include "bsp_gpio.h"
#include "button.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_sleep.h"

static const char* TAG = "BSP_Gpio";

void bsp_gpio_init(void)
{
    // LED + Relay (Output)
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << BSP_LED_RED) | (1ULL << BSP_LED_GREEN) | (1ULL << BSP_RELAY),
        .mode = GPIO_MODE_OUTPUT,
    };
    esp_err_t ret = gpio_config(&io);
    if(ret){
        ESP_LOGE(TAG, "ESP ERROR : %d", ret);
    }
    
    gpio_set_level(BSP_LED_RED, 0);
    gpio_set_level(BSP_LED_GREEN, 0);
    gpio_set_level(BSP_RELAY, 0);   // 릴레이 OFF 보장


    // Button (Input)

    // GPIO 설정
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BSP_MCU_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_install_isr_service(0);

    ESP_LOGI(TAG, "start");

}


