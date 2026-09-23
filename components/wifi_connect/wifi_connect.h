#ifndef __WIFI_CONNECT_H__
#define __WIFI_CONNECT_H__

#include "esp_err.h"

typedef struct WIFI_STA_INIT_SRUCT
{
    char *ssid;
    char *password;
    int timeout_ms;
    uint8_t number_retries;
} wifi_init_sta_t;

void wifi_connect_init(void);
esp_err_t wifi_connect_sta(char *ssid, char *password, int timeout_ms);
esp_err_t wifi_connect_sta_adv(wifi_init_sta_t wifi_init_sta);
void wifi_connect_ap(char *ssid, char *password);
void wifi_disconnect(void);


#endif