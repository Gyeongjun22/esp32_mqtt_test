
#include "led.h"

void led_on(gpio_num_t pin){
    gpio_set_level(pin, true);
}

void led_off(gpio_num_t pin){
    gpio_set_level(pin, false);
}

void led_toggle(gpio_num_t pin){
    int level = gpio_get_level(pin);
    gpio_set_level(pin, !level);
}
