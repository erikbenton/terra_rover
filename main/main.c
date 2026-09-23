#include <stdio.h>
#include <stdlib.h>
#include "sd_card.h"
#include "camera.h"
#include "esp_log.h"

sd_card_t sd_card;
camera_t camera;

void take_photo(camera_t *camera, sd_card_t *sd_card)
{
    camera->grab_photo(camera);
    sd_card->write_data_file(sd_card, "/store/new_photo.jpg", camera->current_photo->buf, camera->current_photo->len);
    camera->release_photo(camera);
    ESP_LOGI("MAIN", "Finished taking photo");
}

void app_main(void)
{
    // init camera first bc setup requires lots of resources
    ESP_ERROR_CHECK(camera_init(&camera, NULL));

    sd_card_init(SD_MMC_TYPE, &sd_card, NULL);

    // quick tests
    sd_card.write_str_file(&sd_card, "/store/text.txt", "Hello, Terra!");
    sd_card.read_line(&sd_card, "/store/text.txt");
    take_photo(&camera, &sd_card);
}
