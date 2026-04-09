


// #include <stdio.h>
// #include <string.h>
// #include "esp_system.h"
// #include "esp_log.h"
// #include "esp_console.h"
// #include "esp_vfs_dev.h"
// #include "esp_vfs_fat.h"
// #include "nvs.h"
// #include "nvs_flash.h"
// #include "cmd_system.h"
// #include "cmd_wifi.h"
// #include "cmd_nvs.h"



// #if SOC_USB_SERIAL_JTAG_SUPPORTED
// #if !CONFIG_ESP_CONSOLE_SECONDARY_NONE
// #warning "A secondary serial console is not useful when using the console component. Please disable it in menuconfig."
// #endif
// #endif

// static const char* TAG = "example";
// #define PROMPT_STR CONFIG_IDF_TARGET
// #define CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH 256
// /* Console command history can be stored to and loaded from a file.
//  * The easiest way to do this is to use FATFS filesystem on top of
//  * wear_levelling library.
//  */
// #if CONFIG_CONSOLE_STORE_HISTORY

// #define MOUNT_PATH "/data"
// #define HISTORY_PATH MOUNT_PATH "/history.txt"

// static void initialize_filesystem(void)
// {
//     static wl_handle_t wl_handle;
//     const esp_vfs_fat_mount_config_t mount_config = {
//             .max_files = 4,
//             .format_if_mount_failed = true
//     };
//     esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
//     if (err != ESP_OK) {
//         ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
//         return;
//     }
// }
// #endif // CONFIG_STORE_HISTORY

// void initialize_nvs(void)
// {
//     esp_err_t err = nvs_flash_init();
//     if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         ESP_ERROR_CHECK( nvs_flash_erase() );
//         err = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(err);
// }
// // 1. 실행할 함수 만들기
// static int do_hello_cmd(int argc, char **argv) {
//     printf("안녕하세요! ESP32 CLI 세상에 오신 것을 환영합니다.\n");
//     return 0;
// }




// void console_main(void)
// {
//     esp_console_repl_t *repl = NULL;
//     esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
//     /* Prompt to be printed before each line.
//      * This can be customized, made dynamic, etc.
//      */
//     repl_config.prompt = PROMPT_STR ">";
//     repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;

// #if CONFIG_CONSOLE_STORE_HISTORY
//     initialize_filesystem();
//     repl_config.history_save_path = HISTORY_PATH;
//     ESP_LOGI(TAG, "Command history enabled");
// #else
//     ESP_LOGI(TAG, "Command history disabled");
// #endif

//     /* Register commands */
//     esp_console_register_help_command();
//     register_system_common();
//     register_system_sleep();
   
//     // 2. 명령어 등록 로직 (console_main 안에 추가)
//     const esp_console_cmd_t hello_cmd = {
//         .command = "hello",
//         .help = "인사말을 출력합니다",
//         .hint = NULL,
//         .func = &do_hello_cmd,
//     };
//     ESP_ERROR_CHECK(esp_console_cmd_register(&hello_cmd));

// #if SOC_WIFI_SUPPORTED
//     register_wifi();
// #endif
//     register_nvs();

// #if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
//     esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

// #elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
//     esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

// #elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
//     esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

// #else
// #error Unsupported console type
// #endif

//     ESP_ERROR_CHECK(esp_console_start_repl(repl));
// }
