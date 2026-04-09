
#include "bsp_gpio.h"
#include "relay.h"

void relay_on(void){
    gpio_set_level(BSP_RELAY, true);
}
void relay_off(void){
    gpio_set_level(BSP_RELAY, false);
}
