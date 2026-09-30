#ifndef __MEMORY_UTILS_H__
#define __MEMORY_UTILS_H__

#include "esp_err.h"
#include "stdint.h"

void log_heap_status(void);
esp_err_t create_heap_status_log_timer(uint32_t period_ms);
void start_heap_status_log_timer(void);
void stop_heap_status_log_timer(void);

#endif