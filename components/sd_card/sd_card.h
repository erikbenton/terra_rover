#ifndef __SD_CARD_H__
#define __SD_CARD_H__

#include "esp_err.h"
#include "sd_protocol_types.h"
#include "esp_http_server.h"

#define SPI_SD_TYPE "spi_sd"
#define SD_MMC_TYPE "sdmmc"

typedef struct SD_CARD_STRUCT sd_card_t;

typedef esp_err_t (*init_sd_card_t)(sd_card_t *self);
typedef esp_err_t (*read_line_t)(sd_card_t *self, const char *path);
typedef esp_err_t (*read_full_file_t)(sd_card_t *self, const char *path, char *buffer, long buff_size);
typedef esp_err_t (*write_str_file_t)(sd_card_t *self, char *path, const char *content);
typedef esp_err_t (*write_data_file_t)(sd_card_t *self, const char *path, const void *data, size_t data_len);
typedef esp_err_t (*stream_web_files_t)(sd_card_t *self, const char *base_dir, const char *base_path, httpd_req_t *req);
typedef esp_err_t (*unmount_t)(sd_card_t *self);

struct SD_CARD_STRUCT
{
    sdmmc_card_t *card;
    init_sd_card_t init;                 // initialize the hardware
    read_line_t read_line;               // read line from file
    read_full_file_t read_full_file;     // read the entire file
    write_str_file_t write_str_file;     // write a string to a file
    write_data_file_t write_data_file;   // write raw data to a file
    stream_web_files_t stream_web_files; // stream files for websites from the sd card
    unmount_t unmount;                   // unmount the sd card
    void *ctx;                           // any extra context for implementing operations
};

void sd_card_init(const char *type, sd_card_t *sd_card, void *ctx);

#endif