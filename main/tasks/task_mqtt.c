#include "task_mqtt.h"
#include "task_event.h"
#include "task_fota.h"
#include "config_nvs.h"

#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "freertos/FreeRTOS.h"

// 구독 토픽
#define TOPIC_RELAY_SET   "esp32/relay/set"   // payload: "on" / "off"
#define TOPIC_LED_SET     "esp32/led/set"     // payload: "on" / "off"
#define TOPIC_FOTA        "esp32/fota"        // payload: http://IP:PORT/firmware.bin

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
        esp_mqtt_client_subscribe(s_client, TOPIC_FOTA, 1);
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
            if (strncmp(data, "on", dlen) == 0)
                xEventGroupSetBits(task_event_get_handle(), EVT_LED_ON);
            else if (strncmp(data, "off", dlen) == 0)
                xEventGroupSetBits(task_event_get_handle(), EVT_LED_OFF);
        } else if (strncmp(topic, TOPIC_FOTA, tlen) == 0) {
            char url[256] = {0};
            int len = dlen < (int)(sizeof(url) - 1) ? dlen : (int)(sizeof(url) - 1);
            strncpy(url, data, len);
            ESP_LOGI("task_mqtt", "FOTA 요청: %s", url);
            task_fota_start(url);
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
    static char uri[128];
    static char cid[64];
    config_nvs_get_mqtt_uri(uri, sizeof(uri));
    config_nvs_get_mqtt_client_id(cid, sizeof(cid));
    uint16_t port = config_nvs_get_mqtt_port();

    esp_mqtt_client_config_t cfg = {
        .broker.address.uri  = uri,
        .broker.address.port = port,
        .credentials.client_id = cid,
        .session.protocol_ver  = MQTT_PROTOCOL_V_5,
        .session.keepalive     = 10,
        .session.disable_keepalive = false,
        .session.last_will = {
            .topic   = TOPIC_STATUS,
            .msg     = "offline",
            .msg_len = 7,
            .qos     = 1,
            .retain  = 1,
        },
        .credentials.username = "jbx",
        .credentials.authentication.password = "456",
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
    };

    s_client = esp_mqtt_client_init(&cfg);

    esp_mqtt5_connection_property_config_t conn_prop = {
        .will_delay_interval = 30,  // 30초 후 LWT 발행 (일시적 끊김 방지)
    };
    esp_mqtt5_client_set_connect_property(s_client, &conn_prop);

    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(s_client);

    ESP_LOGI(TAG, "MQTT client started → %s:%u  id=%s", uri, port, cid);
}

// ── 외부에서 호출 가능한 publish ─────────────────────────────
void mqtt_publish(const char *topic, const char *data)
{
    if (s_client == NULL) return;
    esp_mqtt_client_publish(s_client, topic, data, 0, 1, 0);
}
