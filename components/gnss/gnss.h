#pragma once
#include <esp_log.h>

#include "nmea_rmc.h"
#include "base_types.h"


void parse_data(ASCII_DATA *data, int length);


//--------------------------------------------------
void init_urat(void);
void rx_task();
