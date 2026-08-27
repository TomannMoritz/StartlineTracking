#pragma once
#include <esp_log.h>

#include "base_types.h"


void parse_data(ASCII_DATA *data, int length);


//--------------------------------------------------
void init_gnss_urat(void);
void gnss_rx_task(void *arg);
