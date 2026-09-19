#include "spi_sd_card.h"
#include "sd_card.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "SPI SD";
static const char *BASE_PATH = "/store";

static esp_err_t init_spi_sd_card(sd_card_t *self)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = SD_PIN_MOSI,
        .miso_io_num = SD_PIN_MISO,
        .sclk_io_num = SD_PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1};

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = 7000; // Must adjust based on setup (eg: wire/connection noise)
    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &spi_bus_config, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_PIN_CS;
    slot_config.host_id = host.slot;

    ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &(self->card)));

    sdmmc_card_print_info(stdout, self->card);

    return ESP_OK;
}

static esp_err_t sd_write_file(sd_card_t *self, char *path, const char *content)
{
    ESP_LOGI(TAG, "writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "w");
    fputs(content, file);
    fclose(file);

    return ESP_OK;
}

static esp_err_t sd_read_full_file(sd_card_t *self, const char *path, char *buffer, long buff_size)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");

    // determine file length
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    // empty out the buffer
    memset(buffer, 0, buff_size);

    // read the full length of the file
    size_t read_bytes = fread(buffer, sizeof(char), length, file);
    buffer[read_bytes] = '\0';

    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);

    return ESP_OK;
}

static esp_err_t sd_read_line(sd_card_t *self, const char *path)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");
    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);
    return ESP_OK;
}

static esp_err_t sd_unmount(sd_card_t *self)
{
    return esp_vfs_fat_sdcard_unmount(BASE_PATH, self->card);
}

void spi_sd_card_create(sd_card_t *sd_card, void *ctx)
{
    sd_card->ctx = ctx;
    sd_card->init = init_spi_sd_card;
    sd_card->read_full_file = sd_read_full_file;
    sd_card->read_line = sd_read_line;
    sd_card->write_file = sd_write_file;
    sd_card->unmount = sd_unmount;
}