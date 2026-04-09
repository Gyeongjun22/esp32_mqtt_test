#ifndef __TASK_MQTT_H__
#define __TASK_MQTT_H__

void task_mqtt_init(void);
void mqtt_publish(const char *topic, const char *data);

#endif
