
idf_component_register(
    SRCS 
        "app_main.c"

        "bsp/bsp_gpio.c"

        "drivers/led.c"
        "drivers/relay.c"
        "drivers/button.c"
        "drivers/hwtimer.c"

        "tasks/task_led.c"
        "tasks/task_relay.c"
        "tasks/task_button.c"
        "tasks/task_hwtimer.c"
        "tasks/task_ble.c"
        "tasks/task_event.c"
        
        "tasks/swtimer.c"
        "tasks/console.c"
        "tasks/task_wifi.c"
        "tasks/task_mqtt.c"

    INCLUDE_DIRS
        "."
        "bsp"
        "drivers"
        "tasks"

    REQUIRES
        nvs_flash
        esp_event
        esp_timer
        bt
        driver
        esp_wifi
        mqtt
        esp_netif
        # console
        # fatfs
        # wear_levelling
        # cmd_system
        # cmd_wifi
        # cmd_nvs
        
)
