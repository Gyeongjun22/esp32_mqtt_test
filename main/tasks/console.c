

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "driver/uart.h"
#include "task_led.h"
#include "task_button.h"
#include "swtimer.h"
#include "task_mqtt.h"
#include "config_cli.h"


#define CLI_UART        UART_NUM_0
#define CLI_BUF_SIZE    256

// ── hello ────────────────────────────────────────────────────────
static BaseType_t cmd_hello(char *out, size_t outLen, const char *cmd) {
    snprintf(out, outLen, "Hello from ESP32!\r\n");
    return pdFALSE;
}

static const CLI_Command_Definition_t hello_def = {
    "hello", "hello: 인사 출력\r\n", cmd_hello, 0
};
static BaseType_t cmd_heap( char *out, size_t outLen, const char *cmd )
{
	sprintf( out, "Current free heap %d bytes, minimum ever free heap %d bytes\r\n", ( int ) xPortGetFreeHeapSize(), ( int ) xPortGetMinimumEverFreeHeapSize() );
    return pdFALSE;
}
static const CLI_Command_Definition_t heap_def = {
    "heap", "heap size 출력\r\n", cmd_heap, 0
};

// ── mqtt ────────────────────────────────────────────────────────
static BaseType_t cmd_mqtt(char *out, size_t outLen, const char *cmd) {
    BaseType_t topicLen, dataLen;
    const char *topic = FreeRTOS_CLIGetParameter(cmd, 1, &topicLen);
    const char *data  = FreeRTOS_CLIGetParameter(cmd, 2, &dataLen);

    if (topic == NULL || data == NULL) {
        snprintf(out, outLen, "usage: mqtt <topic> <data>\r\n");
        return pdFALSE;
    }

    // FreeRTOS-CLI 파라미터는 null-terminate 안 됨 → 로컬 버퍼에 복사
    char t[64], d[128];
    snprintf(t, sizeof(t), "%.*s", (int)topicLen, topic);
    snprintf(d, sizeof(d), "%.*s", (int)dataLen,  data);

    mqtt_publish(t, d);
    snprintf(out, outLen, "published: %s → %s\r\n", t, d);
    return pdFALSE;
}

static const CLI_Command_Definition_t mqtt_def = {
    "mqtt", "mqtt <topic> <data>: MQTT 발행\r\n", cmd_mqtt, 2
};

// ── restart ────────────────────────────────────────────────────────
BaseType_t cmd_restart(char *out, size_t outLen, const char *cmd) {
	// ( void ) out;
	// ( void ) outLen;
	// configASSERT( out );
	esp_restart();
    return pdFALSE;
}
static const CLI_Command_Definition_t restart_def = {
    "restart",
    "restart\r\n",
    cmd_restart, 0
};

// ── tasks ────────────────────────────────────────────────────────
// vTaskList 출력(태스크 1개 ~45자 × 최대 30개 = ~1350자) + 헤더
#define TASKS_BUF_SIZE  1400

static BaseType_t cmd_tasks(char *out, size_t outLen, const char *cmd)
{
    static char buf[TASKS_BUF_SIZE];
    static size_t s_offset = 0;

    // 첫 호출: 전체 목록을 buf에 생성
    if (s_offset == 0) {
        int hlen = snprintf(buf, sizeof(buf),
            "Name            State  Pri  Stack  Num\r\n"
            "----------------------------------------------\r\n");
        vTaskList(buf + hlen);
        buf[sizeof(buf) - 1] = '\0';  // 안전 null-terminate
    }

    size_t remaining = strlen(buf + s_offset);
    if (remaining == 0) {
        s_offset = 0;
        out[0] = '\0';
        return pdFALSE;
    }

    size_t chunk = outLen - 1;
    if (remaining <= chunk) {
        // 마지막 조각
        memcpy(out, buf + s_offset, remaining);
        out[remaining] = '\0';
        s_offset = 0;
        return pdFALSE;
    } else {
        // 아직 더 남음 → pdTRUE 로 재호출 요청
        memcpy(out, buf + s_offset, chunk);
        out[chunk] = '\0';
        s_offset += chunk;
        return pdTRUE;
    }
}

static const CLI_Command_Definition_t tasks_def = {
    "tasks",
    "tasks: 태스크 상태 목록 출력\r\n",
    cmd_tasks, 0
};

// ── task-start ───────────────────────────────────────────────────
static BaseType_t cmd_task_start(char *out, size_t outLen, const char *cmd)
{
    BaseType_t nameLen;
    const char *name = FreeRTOS_CLIGetParameter(cmd, 1, &nameLen);
    if (name == NULL) {
        snprintf(out, outLen, "usage: task-start <led|relay|button>\r\n");
        return pdFALSE;
    }

    if (strncmp(name, "led", nameLen) == 0) {
        task_led_init();
        snprintf(out, outLen, "led started\r\n");
    } else if (strncmp(name, "button", nameLen) == 0) {
        task_button_init();
        snprintf(out, outLen, "button started\r\n");
    } else if (strncmp(name, "timer", nameLen) == 0) {
        sw_timer_init();
    } 
    else {
        snprintf(out, outLen, "unknown task: %.*s\r\n", (int)nameLen, name);
    }
    return pdFALSE;
}

static const CLI_Command_Definition_t task_start_def = {
    "task-start",
    "task-start <name>: 태스크 실행 (led, button)\r\n",
    cmd_task_start, 1
};

// ── task-stop ────────────────────────────────────────────────────
static BaseType_t cmd_task_stop(char *out, size_t outLen, const char *cmd)
{
    BaseType_t nameLen;
    const char *name = FreeRTOS_CLIGetParameter(cmd, 1, &nameLen);
    if (name == NULL) {
        snprintf(out, outLen, "usage: task-stop <led|relay|button>\r\n");
        return pdFALSE;
    }

    if (strncmp(name, "led", nameLen) == 0) {
        task_led_stop();
        snprintf(out, outLen, "led stopped\r\n");
    } else if (strncmp(name, "button", nameLen) == 0) {
        task_button_stop();
        snprintf(out, outLen, "button stopped\r\n");
    } else if (strncmp(name, "timer", nameLen) == 0) {
        sw_timer_stop();
    } 
    else {
        snprintf(out, outLen, "unknown task: %.*s\r\n", (int)nameLen, name);
    }
    return pdFALSE;
}

static const CLI_Command_Definition_t task_stop_def = {
    "task-stop",
    "task-stop <name>: 태스크 종료 (led, button)\r\n",
    cmd_task_stop, 1
};

static void console_task(void *arg) {
    char input[CLI_BUF_SIZE];
    char output[CLI_BUF_SIZE];
    int idx = 0;
    uint8_t ch;

    // UART0 드라이버 설치 (UART0은 기본 로그 출력과 공유)
    uart_config_t uart_cfg = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(CLI_UART, &uart_cfg);
    uart_driver_install(CLI_UART, CLI_BUF_SIZE * 2, 0, 0, NULL, 0);

    FreeRTOS_CLIRegisterCommand(&hello_def);
    FreeRTOS_CLIRegisterCommand(&heap_def);
    FreeRTOS_CLIRegisterCommand(&mqtt_def);
    FreeRTOS_CLIRegisterCommand(&restart_def);
    FreeRTOS_CLIRegisterCommand(&tasks_def);
    FreeRTOS_CLIRegisterCommand(&task_start_def);
    FreeRTOS_CLIRegisterCommand(&task_stop_def);
    config_cli_register();

    while (1) {
        if (uart_read_bytes(CLI_UART, &ch, 1, pdMS_TO_TICKS(10)) > 0) {
            uart_write_bytes(CLI_UART, (char*)&ch, 1);  // 에코
            if (ch == '\r' || ch == '\n') {
                input[idx] = '\0';
                uart_write_bytes(CLI_UART, "\r\n", 2);
                if (idx > 0) {
                    BaseType_t more;
                    do {
                        more = FreeRTOS_CLIProcessCommand(input, output, CLI_BUF_SIZE);
                        uart_write_bytes(CLI_UART, output, strlen(output));
                    } while (more != pdFALSE);
                }
                idx = 0;
            }
            else if (ch == '\b' && idx > 0) {
                idx --;
            }
            else if (idx < CLI_BUF_SIZE - 1) {
                input[idx++] = ch;
            }
        }
    }
}

void console_init(void) {
    xTaskCreate(console_task, "cli", 4096, NULL, 3, NULL);
}
