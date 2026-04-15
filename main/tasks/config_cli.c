#include "config_cli.h"
#include "config_nvs.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "esp_log.h"

static const char *TAG = "config_cli";

/*
 * config mem show
 * config mem save wifi <ssid> <pass>
 * config mem save mqtt <uri> <port> <client_id>
 */
static BaseType_t cmd_config(char *out, size_t outLen, const char *cmd)
{
    BaseType_t p1Len, p2Len, p3Len, p4Len, p5Len, p6Len;
    const char *p1 = FreeRTOS_CLIGetParameter(cmd, 1, &p1Len); // "mem"
    const char *p2 = FreeRTOS_CLIGetParameter(cmd, 2, &p2Len); // "show" | "save"
    const char *p3 = FreeRTOS_CLIGetParameter(cmd, 3, &p3Len); // "wifi" | "mqtt"
    const char *p4 = FreeRTOS_CLIGetParameter(cmd, 4, &p4Len); // ssid | uri
    const char *p5 = FreeRTOS_CLIGetParameter(cmd, 5, &p5Len); // pass | port
    const char *p6 = FreeRTOS_CLIGetParameter(cmd, 6, &p6Len); // (mqtt) client_id

    if (p1 == NULL || strncmp(p1, "mem", p1Len) != 0) {
        snprintf(out, outLen,
            "usage:\r\n"
            "  config mem show\r\n"
            "  config mem save wifi <ssid> <pass>\r\n"
            "  config mem save mqtt <uri> <port> <client_id>\r\n");
        return pdFALSE;
    }

    // config mem show
    if (p2 != NULL && strncmp(p2, "show", p2Len) == 0) {
        char ssid[64], pass[64], uri[128], cid[64];
        config_nvs_get_wifi_ssid(ssid, sizeof(ssid));
        config_nvs_get_wifi_pass(pass, sizeof(pass));
        config_nvs_get_mqtt_uri(uri, sizeof(uri));
        config_nvs_get_mqtt_client_id(cid, sizeof(cid));
        uint16_t port = config_nvs_get_mqtt_port();

        snprintf(out, outLen,
            "[WiFi]\r\n"
            "  ssid : %s\r\n"
            "  pass : %s\r\n"
            "[MQTT]\r\n"
            "  uri  : %s\r\n"
            "  port : %u\r\n"
            "  id   : %s\r\n",
            ssid[0] ? ssid : "(미설정)",
            pass[0] ? "********" : "(미설정)",
            uri, port, cid);
        return pdFALSE;
    }

    // config mem save wifi <ssid> <pass>
    if (p2 != NULL && strncmp(p2, "save", p2Len) == 0 &&
        p3 != NULL && strncmp(p3, "wifi", p3Len) == 0) {
        if (p4 == NULL || p5 == NULL) {
            snprintf(out, outLen, "usage: config mem save wifi <ssid> <pass>\r\n");
            return pdFALSE;
        }
        char ssid[64], pass[64];
        snprintf(ssid, sizeof(ssid), "%.*s", (int)p4Len, p4);
        snprintf(pass, sizeof(pass), "%.*s", (int)p5Len, p5);
        config_nvs_set_wifi_ssid(ssid);
        config_nvs_set_wifi_pass(pass);
        snprintf(out, outLen, "WiFi 설정 저장 완료. restart 후 적용됩니다.\r\n");
        return pdFALSE;
    }

    // config mem save mqtt <uri> <port> <client_id>
    if (p2 != NULL && strncmp(p2, "save", p2Len) == 0 &&
        p3 != NULL && strncmp(p3, "mqtt", p3Len) == 0) {
        if (p4 == NULL || p5 == NULL || p6 == NULL) {
            snprintf(out, outLen, "usage: config mem save mqtt <uri> <port> <client_id>\r\n");
            return pdFALSE;
        }
        char uri[128], port_str[8], cid[64];
        snprintf(uri,      sizeof(uri),      "%.*s", (int)p4Len, p4);
        snprintf(port_str, sizeof(port_str), "%.*s", (int)p5Len, p5);
        snprintf(cid,      sizeof(cid),      "%.*s", (int)p6Len, p6);
        config_nvs_set_mqtt_uri(uri);
        config_nvs_set_mqtt_port((uint16_t)atoi(port_str));
        config_nvs_set_mqtt_client_id(cid);
        snprintf(out, outLen, "MQTT 설정 저장 완료. restart 후 적용됩니다.\r\n");
        return pdFALSE;
    }

    snprintf(out, outLen,
        "usage:\r\n"
        "  config mem show\r\n"
        "  config mem save wifi <ssid> <pass>\r\n"
        "  config mem save mqtt <uri> <port> <client_id>\r\n");
    return pdFALSE;
}

static const CLI_Command_Definition_t config_def = {
    "config",
    "config mem show|save wifi|mqtt ...: NVS 설정 조회/저장\r\n",
    cmd_config,
    -1  // 가변 인자
};

void config_cli_register(void)
{
    FreeRTOS_CLIRegisterCommand(&config_def);
    ESP_LOGI(TAG, "config 명령 등록 완료");
}
