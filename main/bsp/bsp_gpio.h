#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "driver/gpio.h"

#define BSP_LED_RED     GPIO_NUM_19
#define BSP_LED_GREEN   GPIO_NUM_22
#define BSP_RELAY       GPIO_NUM_23
#define BSP_MCU_BUTTON  GPIO_NUM_4

void bsp_gpio_init(void);

#endif
