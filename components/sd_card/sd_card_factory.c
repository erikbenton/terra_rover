#include "sd_card.h"
#include "string.h"
#include "esp_log.h"
#include "spi_sd_card.h"
#include "sd_mmc_card.h"

const char *TAG = "SD_FACTORY";

void sd_card_init(const char *type, sd_card_t *sd_card, void *ctx)
{
    // create appropriate hardware
    if (strcmp(type, SPI_SD_TYPE) == 0)
    {
        ESP_LOGI(TAG, "Creating SPI SD card");
        spi_sd_card_create(sd_card, ctx);
    }

    if (strcmp(type, SD_MMC_TYPE) == 0)
    {
        ESP_LOGI(TAG, "Creating SD MMC card");
        sd_mmc_card_create(sd_card, ctx);
    }

    // inititalize the sd card
    sd_card->init(sd_card);
}
