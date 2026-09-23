#include "sd_card.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

static const char *TAG = "SDMMC";
static const char *BASE_PATH = "/store";

static esp_err_t init_sd_mmc_card(sd_card_t *self)
{
    // configure SDMMC mount for FAT
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 16 * 1024};

    // should be the defaults for the built-in card reader
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t sdmmc_slot = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP; // double check pullups are enabled

    ESP_ERROR_CHECK(
        esp_vfs_fat_sdmmc_mount(
            BASE_PATH,
            &host,
            &sdmmc_slot,
            &mount_config,
            &(self->card)));

    sdmmc_card_print_info(stdout, self->card);

    return ESP_OK;
}

static esp_err_t sd_write_file(sd_card_t *self, char *path, const char *content)
{
    ESP_LOGI(TAG, "writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        ESP_LOGE(TAG, "Unable to get a file to write to.");
        return ESP_FAIL;
    }

    fputs(content, file);
    fclose(file);

    return ESP_OK;
}

static esp_err_t sd_read_full_file(sd_card_t *self, const char *path, char *buffer, long buff_size)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        ESP_LOGE(TAG, "Unable to get file to read.");
        return ESP_FAIL;
    }

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

    if (file == NULL)
    {
        perror("error opening file");
        ESP_LOGE(TAG, "Unable to get file to read.");
        return ESP_FAIL;
    }

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);
    return ESP_OK;
}

static esp_err_t sd_write_data_file(sd_card_t *self, const char *path, const void *data, size_t data_len)
{
    FILE *file = fopen(path, "w");

    if (file == NULL)
    {
        perror("error opening file");
        ESP_LOGE(TAG, "Unable to get a file handle to write to.");
        fclose(file);
        return ESP_FAIL;
    }

    fwrite(data, sizeof(uint8_t), data_len, file);
    fclose(file);

    return ESP_OK;
}

static esp_err_t sd_unmount(sd_card_t *self)
{
    return esp_vfs_fat_sdcard_unmount(BASE_PATH, self->card);
}

void sd_mmc_card_create(sd_card_t *sd_card, void *ctx)
{
    sd_card->ctx = ctx;
    sd_card->init = init_sd_mmc_card;
    sd_card->read_full_file = sd_read_full_file;
    sd_card->read_line = sd_read_line;
    sd_card->write_str_file = sd_write_file;
    sd_card->write_data_file = sd_write_data_file;
    sd_card->unmount = sd_unmount;
}