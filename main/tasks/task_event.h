#ifndef __TASKS_EVENT_H__
#define __TASKS_EVENT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define EVT_TIMER               (1 << 0)    // 1
#define EVT_BUTTON              (1 << 1)    // 2
#define EVT_BUTTON_LONG         (1 << 2)    // 4
#define EVT_BUTTON_LONG_LONG    (1 << 3)    // 8
#define EVT_RELAY_ON            (1 << 4)    // 16  (MQTT 제어)
#define EVT_RELAY_OFF           (1 << 5)    // 32  (MQTT 제어)
#define EVT_LED_ON              (1 << 6)    // 64  (MQTT 제어)
#define EVT_LED_OFF             (1 << 7)    // 128 (MQTT 제어)

void task_event_init(void);
EventGroupHandle_t task_event_get_handle(void);

#endif
