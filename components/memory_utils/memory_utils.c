#include "memory_utils.h"
#include <stdlib.h>
#include <stdint.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_timer.h"

static TimerHandle_t heap_status_timer;

void log_heap_status_cb(TimerHandle_t xTimer)
{
    log_heap_status();
}

void log_heap_status(void)
{
    size_t free = esp_get_free_heap_size();
    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    float frag = 1.0f - ((float)largest / (float)free);
    ESP_LOGI("HEAP", "Free: %u | Largest block: %u | Frag: %.2f%%",
             free, largest, frag * 100.0f);
}

esp_err_t create_heap_status_log_timer(uint32_t period_ms)
{
    heap_status_timer = xTimerCreate(
        "Heap Status Log Timer",
        pdMS_TO_TICKS(period_ms),
        pdTRUE,
        (void *)0,
        log_heap_status_cb);

    return heap_status_timer == NULL ? ESP_FAIL : ESP_OK;
}

void start_heap_status_log_timer(void)
{
    xTimerStart(heap_status_timer, portMAX_DELAY);
}

void stop_heap_status_log_timer(void)
{
    xTimerStop(heap_status_timer, portMAX_DELAY);
}