#include <stdio.h>
#include <stdlib.h>
#include "sd_card.h"
#include "camera.h"
#include "esp_log.h"
#include "wifi_connect.h"
#include "nvs_flash.h"
#include "server.h"
#include "memory_utils.h"

sd_card_t sd_card;
camera_t camera;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    // init camera first bc setup requires lots of resources
    ESP_ERROR_CHECK(camera_init(&camera, NULL));

    sd_card_init(SD_MMC_TYPE, &sd_card, NULL);

    wifi_connect_init();

    // Configure the Wifi SSID and password in the menuconfig
    wifi_connect_sta(
        CONFIG_WIFI_SSID,
        CONFIG_WIFI_PASSWORD,
        10000);

    server_init(&sd_card, &camera);

#ifdef CONFIG_HEAP_MEM_DEBUG
    create_heap_status_log_timer(CONFIG_HEAP_MEM_PERIOD);
    start_heap_status_log_timer();
#endif
}
