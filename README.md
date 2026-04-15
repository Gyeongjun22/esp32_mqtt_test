# ESP32 MQTT v5 Test Project

ESP32-WROVER-KIT 기반 임베디드 애플리케이션.
LED, Relay, Button, HW/SW Timer, BLE, WiFi STA+AP, MQTT v5, FOTA, FreeRTOS CLI 기능을 포함합니다.

## 개발 환경

| 항목 | 내용 |
|------|------|
| Target | ESP32 (Xtensa) |
| Board | ESP32-WROVER-KIT 3.3V |
| Framework | ESP-IDF v5.2.1 |
| RTOS | FreeRTOS |
| Serial Port | COM13 |

## 프로젝트 구조

```
├── main/
│   ├── app_main.c              진입점 — NVS, BSP, 태스크 초기화
│   ├── bsp/
│   │   └── bsp_gpio.h/c        GPIO 핀 정의
│   ├── drivers/
│   │   ├── led.c/h             LED on/off/toggle
│   │   ├── relay.c/h           Relay on/off
│   │   ├── button.c/h          GPIO 입력 + ISR
│   │   └── hwtimer.c/h         Hardware timer 설정
│   └── tasks/
│       ├── task_event.c/h      EventGroup 허브 — LED/Relay/MQTT 이벤트 통합 처리
│       ├── task_led.c/h        LED 블링크 태스크 (300ms on / 100ms off)
│       ├── task_button.c/h     버튼 Short/Long/LongLong 감지
│       ├── task_hwtimer.c/h    Hardware timer 태스크
│       ├── task_ble.c/h        BLE Peripheral (GATT attribute table)
│       ├── task_wifi.c/h       WiFi STA+AP 듀얼모드
│       ├── task_mqtt.c/h       MQTT v5 클라이언트 (TLS, LWT, Keepalive)
│       ├── task_fota.c/h       OTA 업데이트 (HTTPS)
│       ├── config_nvs.c/h      NVS 설정 저장/읽기 (WiFi, MQTT)
│       ├── config_cli.c/h      CLI config 명령 핸들러
│       ├── swtimer.c/h         Software timer 유틸리티
│       └── console.c/h         FreeRTOS-CLI UART 콘솔
├── components/
│   └── freertos_cli/           FreeRTOS-Plus-CLI 라이브러리
├── partitions.csv              플래시 레이아웃
└── CMakeLists.txt
```

## GPIO 핀 맵

| 신호 | GPIO |
|------|------|
| LED Red | 19 |
| LED Green | 22 |
| Relay | 23 |
| Button | 4 |

## 빌드 및 플래시

```bash
# 환경 설정 (셸 세션마다 1회)
export IDF_PATH=D:/esp32_sub/esp-idf-v5.2.1/esp-idf-v5.2.1
source $IDF_PATH/export.sh

idf.py build                     # 빌드
idf.py -p COM13 flash            # 플래시
idf.py -p COM13 monitor          # 시리얼 모니터
idf.py -p COM13 flash monitor    # 플래시 + 모니터
idf.py fullclean                 # 빌드 아티팩트 전체 삭제
```

## CLI 명령어

UART0(115200bps)으로 접속하면 사용할 수 있습니다. 입력 버퍼는 255자입니다.

| 명령 | 인자 | 설명 |
|------|------|------|
| `help` | — | 전체 명령어 목록 출력 |
| `hello` | — | 인사 출력 |
| `heap` | — | 현재/최소 여유 힙 출력 |
| `tasks` | — | 전체 태스크 상태 및 스택 잔량 출력 |
| `task-start` | `<name>` | 태스크 시작 (`led` / `button`) |
| `task-stop` | `<name>` | 태스크 종료 (`led` / `button`) |
| `mqtt` | `<topic> <data>` | 지정 토픽으로 MQTT 발행 |
| `restart` | — | 재부팅 |
| `config mem show` | — | NVS 저장값 출력 |
| `config mem save wifi` | `<ssid> <pass>` | WiFi 설정 저장 |
| `config mem save mqtt` | `<uri> <port> <id>` | MQTT 설정 저장 |

### tasks 출력 예시

```
Name            State  Pri  Stack  Num
----------------------------------------------
cli             X       3    2872   21
Task_led        B      11    1568   19
mqtt_task       B       5    4168    1
...
```

State: `R`=Ready, `B`=Blocked, `X`=Running, `S`=Suspended  
Stack: 남은 워드 수 — **200 이하면 오버플로 위험**

## WiFi

STA + AP 듀얼모드로 동작합니다.

| 모드 | 설명 |
|------|------|
| STA | NVS에 저장된 SSID/PW로 공유기 접속 |
| AP | `ESP32-AP` / `12345678` 핫스팟 제공 (최대 4대) |

초기 WiFi 설정:
```
config mem save wifi MySSID MyPassword
restart
```

## MQTT v5

### 브로커 연결

EMQX Cloud 등 외부 브로커 사용 시 TLS(`mqtts://`) 지원합니다.

```
config mem save mqtt mqtts://your-id.emqx.cloud 8883 esp32-a02
restart
```

### LWT (Last Will and Testament)

| 항목 | 값 |
|------|-----|
| 토픽 | `esp32/status` |
| 비정상 종료 payload | `offline` (retain) |
| 정상 연결 payload | `online` |
| will_delay_interval | 30초 (일시적 끊김 오탐 방지) |
| Keepalive | 10초 (5초마다 PING) |

### 토픽 구조

| 방향 | 토픽 | payload |
|------|------|---------|
| Subscribe | `esp32/relay/set` | `on` / `off` |
| Subscribe | `esp32/led/set` | `on` / `off` |
| Subscribe | `esp32/fota` | `http://IP:PORT/firmware.bin` |
| Publish | `esp32/status` | `online` / `offline` |
| Publish | `esp32/button` | `short` / `long` / `long_long` |
| Publish | `esp32/fota/status` | `started` / `success` / `failed` |

## 이벤트 구조

`task_event`가 EventGroup 허브 역할을 합니다. 모든 태스크는 비트를 설정하고, `task_event`가 처리합니다.

| 이벤트 비트 | 발생 | 동작 |
|------------|------|------|
| `EVT_BUTTON` | 버튼 Short press | LED Green ON, Relay ON |
| `EVT_TIMER` | SW/HW 타이머 | LED OFF, Relay OFF |
| `EVT_BUTTON_LONG` | 버튼 Long press | LED 태스크 토글 |
| `EVT_BUTTON_LONG_LONG` | 버튼 Long Long press | 태스크 목록 출력 |
| `EVT_RELAY_ON` | MQTT relay/set on | Relay ON |
| `EVT_RELAY_OFF` | MQTT relay/set off | Relay OFF |
| `EVT_LED_ON` | MQTT led/set on | LED Red ON |
| `EVT_LED_OFF` | MQTT led/set off | LED Red OFF |

## 버튼 동작

| 누름 시간 | 이벤트 | 동작 |
|-----------|--------|------|
| 50ms ~ 2초 | Short press | LED Green ON, Relay ON, MQTT 발행 |
| 2 ~ 5초 | Long press | LED 블링크 태스크 토글 |
| 5초 이상 | Long Long press | 태스크 목록 출력 |

## FOTA

MQTT `esp32/fota` 토픽으로 펌웨어 URL 수신 시 자동 OTA 업데이트:

```bash
mosquitto_pub -t "esp32/fota" -m "http://192.168.0.100:8080/firmware.bin"
```

- 성공 시 `esp32/fota/status` → `success` 발행 후 자동 재부팅
- 실패 시 `esp32/fota/status` → `failed` 발행, 현재 펌웨어 유지

## 플래시 레이아웃

| 파티션 | 타입 | 크기 | 용도 |
|--------|------|------|------|
| nvs | data/nvs | 24KB | WiFi/MQTT 설정 저장 |
| phy_init | data/phy | 4KB | RF 초기화 |
| otadata | data/ota | 8KB | OTA 부팅 선택 |
| ota_0 | app/ota_0 | 1.5MB | 펌웨어 슬롯 0 |
| ota_1 | app/ota_1 | 1.5MB | 펌웨어 슬롯 1 |
| storage | data/fat | 896KB | FAT 파티션 (미사용, 예약) |

## menuconfig 필수 설정

```
# MQTT v5 지원
Component config → ESP-MQTT → Enable MQTT 5.0 support  [활성화]

# tasks 명령어용 (vTaskList 사용)
Component config → FreeRTOS → Kernel → Enable FreeRTOS trace facility             [활성화]
Component config → FreeRTOS → Kernel → Enable FreeRTOS stats formatting functions  [활성화]
```
