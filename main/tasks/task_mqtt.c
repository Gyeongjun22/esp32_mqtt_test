#include "task_mqtt.h"
#include "task_event.h"
#include "led.h"
#include "bsp_gpio.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

// ── 설정 ──────────────────────────────────────────────────────
#define MQTT_BROKER_URI   "mqtt://192.168.1.100"  // ← Mosquitto 브로커 IP로 변경
#define MQTT_PORT         1883
#define MQTT_CLIENT_ID    "esp32-a02"

// 구독 토픽
#define TOPIC_RELAY_SET   "esp32/relay/set"   // payload: "on" / "off"
#define TOPIC_LED_SET     "esp32/led/set"     // payload: "on" / "off"

// 발행 토픽
#define TOPIC_STATUS      "esp32/status"
#define TOPIC_BUTTON      "esp32/button"

static const char *TAG = "task_mqtt";
static esp_mqtt_client_handle_t s_client = NULL;

// ── 이벤트 핸들러 ─────────────────────────────────────────────
static void mqtt_event_handler(void *arg, esp_event_base_t base,
                                int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t ev = (esp_mqtt_event_handle_t)event_data;

    switch (event_id) {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected (MQTT v5)");
        esp_mqtt_client_subscribe(s_client, TOPIC_RELAY_SET, 1);
        esp_mqtt_client_subscribe(s_client, TOPIC_LED_SET, 1);
        mqtt_publish(TOPIC_STATUS, "online");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected");
        break;

    case MQTT_EVENT_DATA: {
        // topic/data 는 NULL-terminate 되어 있지 않으므로 길이로 비교
        int tlen = ev->topic_len;
        int dlen = ev->data_len;
        const char *topic = ev->topic;
        const char *data  = ev->data;

        ESP_LOGI(TAG, "RX  topic=%.*s  data=%.*s", tlen, topic, dlen, data);

        if (strncmp(topic, TOPIC_RELAY_SET, tlen) == 0) {
            if (strncmp(data, "on", dlen) == 0)
                xEventGroupSetBits(task_event_get_handle(), EVT_RELAY_ON);
            else if (strncmp(data, "off", dlen) == 0)
                xEventGroupSetBits(task_event_get_handle(), EVT_RELAY_OFF);

        } else if (strncmp(topic, TOPIC_LED_SET, tlen) == 0) {
            if (strncmp(data, "on", dlen) == 0) {
                xEventGroupSetBits(task_event_get_handle(), EVT_LED_ON);
                led_on(BSP_LED_RED);
            } else if (strncmp(data, "off", dlen) == 0) {
                xEventGroupSetBits(task_event_get_handle(), EVT_LED_OFF);
                led_off(BSP_LED_RED);
            }
        }
        break;
    }

    case MQTT_EVENT_ERROR:
        if (ev->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "TCP error: %d", ev->error_handle->esp_tls_last_esp_err);
        }
        break;

    default:
        break;
    }
}

// ── 초기화 ────────────────────────────────────────────────────
void task_mqtt_init(void)
{
    esp_mqtt_client_config_t cfg = {
        .broker.address.uri  = MQTT_BROKER_URI,
        .broker.address.port = MQTT_PORT,
        .credentials.client_id = MQTT_CLIENT_ID,
        .session.protocol_ver  = MQTT_PROTOCOL_V_5,   // MQTT v5
        .session.keepalive     = 30,
    };

    s_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(s_client);

    ESP_LOGI(TAG, "MQTT client started → %s:%d", MQTT_BROKER_URI, MQTT_PORT);
}

// ── 외부에서 호출 가능한 publish ─────────────────────────────
void mqtt_publish(const char *topic, const char *data)
{
    if (s_client == NULL) return;
    esp_mqtt_client_publish(s_client, topic, data, 0, 1, 0);
}
