#pragma once
#include <freertos/FreeRTOS.h>
#include "base_types.h"

// Note: start countdown with negative value and count upwards
typedef struct {
    uint8_t is_running;
    int32_t seconds;
    TickType_t previous_tick;
} CountdownTimer;

//--------------------------------------------------
void init_countdown(void);


//--------------------------------------------------
void start_countdown(void);


//--------------------------------------------------
// Logging
//--------------------------------------------------
void countdown_start_stop_task(void *);
void log_countdown_timer(FILE *log_fd, CountdownTimer *countdown_timer);
