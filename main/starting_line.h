
#pragma once
#include <esp_log.h>

#include "ring_buffer.h"


typedef struct {
    Coordinate committee_boat;
    Coordinate pin_end;
} StartingLine;


//--------------------------------------------------
void init_starting_line();

void set_committee_boat_task(void *arg);
void set_pin_end_task(void *arg);


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_starting_line(FILE *log_fd, StartingLine *starting_line);
