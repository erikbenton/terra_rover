#include "string.h"
#include "wifi_connect.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static esp_netif_t *esp_netif;
static EventGroupHandle_t wifi_events;
static char *TAG = "WIFI CONNECT";
static int CONNECTED = BIT0;
static int DISCONNECTED = BIT1;

char *get_wifi_disconnection_string(wifi_err_reason_t wifi_err_reason);

static bool attempt_to_reconnect = false;
const uint8_t default_max_retries = 5;
uint8_t max_retries = default_max_retries;
uint8_t reconnect_retries = 0;

// refer to ESP docs for expected order of events: ESP32 Wi-Fi Station General Scenario
void event_handler(void *event_handler_arg,
                   esp_event_base_t event_base,
                   int32_t event_id,
                   void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
        reconnect_retries = 0;
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
    {
        wifi_event_sta_disconnected_t *wifi_event_sta_disconnected = event_data;
        ESP_LOGW(
            TAG,
            "DISCONNECTED %d - %s",
            wifi_event_sta_disconnected->reason,
            get_wifi_disconnection_string(wifi_event_sta_disconnected->reason));

        // reconnect logic for manageable events
        if (attempt_to_reconnect &&
            (wifi_event_sta_disconnected->reason == WIFI_REASON_NO_AP_FOUND ||
             wifi_event_sta_disconnected->reason == WIFI_REASON_ASSOC_LEAVE ||
             wifi_event_sta_disconnected->reason == WIFI_REASON_AUTH_EXPIRE))
        {
            if (reconnect_retries < max_retries)
            {
                vTaskDelay(pdMS_TO_TICKS(5000));
                ESP_LOGI(TAG, "Attempting reconnect: %d of %d", reconnect_retries + 1, max_retries);
                esp_wifi_connect();
                reconnect_retries++;
            }
            else
            {
                ESP_LOGW(TAG, "Reconnect attempts have failed");
            }
        }

        xEventGroupSetBits(wifi_events, DISCONNECTED);
        break;
    }
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
        xEventGroupSetBits(wifi_events, CONNECTED);
        break;
    default:
        break;
    }
}

void wifi_connect_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

esp_err_t wifi_connect_sta(char *ssid, char *password, int timeout_ms)
{
    attempt_to_reconnect = true;
    max_retries = default_max_retries;
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // allow for "forever" waits if timeout_ms is -1
    int timeout = timeout_ms == -1 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);

    EventBits_t result = xEventGroupWaitBits(
        wifi_events,
        (CONNECTED | DISCONNECTED),
        true,
        false,
        timeout);

    return result == CONNECTED ? ESP_OK : ESP_FAIL;
}

esp_err_t wifi_connect_sta_adv(wifi_init_sta_t wifi_init_sta)
{
    attempt_to_reconnect = true;
    max_retries = wifi_init_sta.number_retries;
    wifi_events = xEventGroupCreate();
    esp_netif = esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, wifi_init_sta.ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, wifi_init_sta.password, sizeof(wifi_config.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // allow for "forever" waits if timeout_ms is -1
    int timeout = wifi_init_sta.timeout_ms == -1
                      ? portMAX_DELAY
                      : pdMS_TO_TICKS(wifi_init_sta.timeout_ms);

    EventBits_t result = xEventGroupWaitBits(
        wifi_events,
        (CONNECTED | DISCONNECTED),
        true,
        false,
        timeout);

    return result == CONNECTED ? ESP_OK : ESP_FAIL;
}

void wifi_connect_ap(char *ssid, char *password)
{
    esp_netif = esp_netif_create_default_wifi_ap();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    strncpy((char *)wifi_config.ap.password, password, sizeof(wifi_config.ap.password) - 1);
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.beacon_interval = 100;
    wifi_config.ap.channel = 1;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_disconnect(void)
{
    attempt_to_reconnect = false;
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_netif_destroy(esp_netif);
}