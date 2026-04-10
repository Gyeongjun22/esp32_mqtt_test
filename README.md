# ESP32 MQTT v5 Test Project

ESP32-WROVER-KIT 기반 임베디드 애플리케이션.
LED, Relay, Button, HW/SW Timer, BLE, WiFi, MQTT v5, FreeRTOS CLI 기능을 포함합니다.

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
│       ├── task_event.c/h      EventGroup — 태스크 간 통신 허브
│       ├── task_led.c/h        LED 100ms 블링크 태스크
│       ├── task_button.c/h     버튼 Short/Long/LongLong 감지
│       ├── task_relay.c/h      Relay 제어 태스크
│       ├── task_hwtimer.c/h    Hardware timer 태스크
│       ├── task_ble.c/h        BLE Peripheral (GATT attribute table)
│       ├── task_wifi.c/h       WiFi STA 연결
│       ├── task_mqtt.c/h       MQTT v5 클라이언트
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

UART0(115200bps)으로 접속하면 사용할 수 있습니다.

| 명령 | 인자 | 설명 |
|------|------|------|
| `help` | — | 전체 명령어 목록 출력 |
| `hello` | — | 인사 출력 |
| `tasks` | — | 전체 태스크 상태 및 스택 잔량 출력 |
| `task-start` | `<name>` | 태스크 시작 (`led` / `relay` / `button`) |
| `task-stop` | `<name>` | 태스크 종료 (`led` / `relay` / `button`) |
| `mqtt` | `<topic> <data>` | 지정 토픽으로 MQTT 발행 |

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

## MQTT v5

### 설정 변경 위치

| 파일 | 항목 |
|------|------|
| `main/tasks/task_wifi.c` | `WIFI_SSID`, `WIFI_PASS` |
| `main/tasks/task_mqtt.c` | `MQTT_BROKER_URI` (Mosquitto 브로커 IP) |

### 토픽 구조

| 방향 | 토픽 | payload |
|------|------|---------|
| Subscribe | `esp32/relay/set` | `on` / `off` |
| Subscribe | `esp32/led/set` | `on` / `off` |
| Publish | `esp32/status` | `online` |
| Publish | `esp32/button` | `short` / `long` / `long_long` |

### Mosquitto 테스트

```bash
# 브로커 실행 (PC)
mosquitto -v

# 전체 토픽 수신
mosquitto_sub -h 192.168.0.140 -V 5 -t "esp32/#" -v -q 1

# Relay 제어
mosquitto_pub -h 192.168.0.140 -V 5 -t "esp32/relay/set" -m "on"
mosquitto_pub -h 192.168.0.140 -V 5 -t "esp32/relay/set" -m "off"

# LED 제어
mosquitto_pub -h 192.168.0.140 -V 5 -t "esp32/led/set" -m "on"
mosquitto_pub -h 192.168.0.140 -V 5 -t "esp32/led/set" -m "off"
```

## 버튼 동작

| 누름 시간 | 동작 | MQTT 발행 |
|-----------|------|-----------|
| 50ms ~ 2초 | Short press | `esp32/button` → `short` |
| 2 ~ 5초 | Long press | `esp32/button` → `long` |
| 5초 이상 | Long Long press | `esp32/button` → `long_long` |

## menuconfig 필수 설정

```
# MQTT v5 지원
Component config → ESP-MQTT → Enable MQTT 5.0 support  [활성화]

# tasks 명령어용 (vTaskList 사용)
Component config → FreeRTOS → Kernel → Enable FreeRTOS trace facility        [활성화]
Component config → FreeRTOS → Kernel → Enable FreeRTOS stats formatting functions  [활성화]

# Timer Service 스택 (기본 2048 → 부족할 경우 증가)
Component config → FreeRTOS → Kernel → Timer task stack size  [4096 권장]
```
