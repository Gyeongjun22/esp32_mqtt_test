#pragma once
#include <stdbool.h>
#include "driver/gpio.h"


void led_on(gpio_num_t pin);
void led_off(gpio_num_t pin);
void led_toggle(gpio_num_t pin);

