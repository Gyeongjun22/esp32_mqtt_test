idf_component_register(
    SRCS "FreeRTOS_CLI.c"
    INCLUDE_DIRS "."
    REQUIRES freertos
)

target_include_directories(${COMPONENT_LIB} PRIVATE
    "$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos"
    "$ENV{IDF_PATH}/components/freertos/esp_additions/include/freertos"
)
