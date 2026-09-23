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
    ESP_ERROR_CHECK(
        wifi_connect_sta(
            CONFIG_WIFI_SSID,
            CONFIG_WIFI_PASSWORD,
            10000));

    // init the server with sd card for site files
    server_init(&sd_card);

    // quick tests
    sd_card.write_str_file(&sd_card, "/store/text.txt", "Hello, Terra!");
    sd_card.read_line(&sd_card, "/store/text.txt");
    take_photo(&camera, &sd_card, "/store/first.jpg");
    take_photo(&camera, &sd_card, "/store/second.jpg");
}
