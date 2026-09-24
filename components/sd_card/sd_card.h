#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include "esp_err.h"
#include "sd_protocol_types.h"
#include "esp_http_server.h"

#define SPI_SD_TYPE "spi_sd"
#define SD_MMC_TYPE "sdmmc"

typedef struct SD_CARD_STRUCT sd_card_t;

struct SD_CARD_STRUCT
{
    sdmmc_card_t *card;
    esp_err_t (*init)(sd_card_t *self);                                                                            // initialize the hardware
    esp_err_t (*read_line)(sd_card_t *self, const char *path);                                                     // read line from file
    esp_err_t (*read_full_file)(sd_card_t *self, const char *path, char *buffer, long buff_size);                  // read the entire file
    esp_err_t (*write_str_file)(sd_card_t *self, char *path, const char *content);                                 // write a string to a file
    esp_err_t (*write_data_file)(sd_card_t *self, const char *path, const void *data, size_t data_len);            // write raw data to a file
    esp_err_t (*stream_web_files)(sd_card_t *self, const char *base_dir, const char *base_path, httpd_req_t *req); // stream files for websites from the sd card
    esp_err_t (*unmount)(sd_card_t *self);                                                                         // unmount the sd card
    void *ctx;                                                                                                     // any extra context for implementing operations
};

void sd_card_init(const char *type, sd_card_t *sd_card, void *ctx);

#endif