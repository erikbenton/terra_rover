#include <stdio.h>
#include <stdlib.h>
#include "sd_card.h"
#include "camera.h"
#include "esp_log.h"
#include "wifi_connect.h"
#include "nvs_flash.h"
#include "server.h"

sd_card_t sd_card;
camera_t camera;

void take_photo(camera_t *camera, sd_card_t *sd_card, const char *photo_path)
{
    camera->grab_photo(camera);
    sd_card->write_data_file(sd_card, photo_path, camera->current_photo->buf, camera->current_photo->len);
    camera->release_photo(camera);
    ESP_LOGI("MAIN", "Finished taking photo");
}

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

    // quick tests
    take_photo(&camera, &sd_card, "/store/photos/first.jpg");
    take_photo(&camera, &sd_card, "/store/photos/second.jpg");
}
