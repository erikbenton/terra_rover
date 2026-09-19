#include <stdio.h>
#include "sd_card.h"

sd_card_t sd_card;

void app_main(void)
{
    sd_card_init(SD_MMC_TYPE, &sd_card, NULL);

    sd_card.write_file(&sd_card, "/store/text.txt", "Hello, Terra!");
    sd_card.read_line(&sd_card, "/store/text.txt");

    ESP_ERROR_CHECK(
        sd_card.unmount(&sd_card));
}
