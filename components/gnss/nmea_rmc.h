// RMC - Recommended Minimum Navigation Information

#pragma once
#include "base_types.h"

extern const uint8_t NMEA_RMC_MSG[4];


typedef struct {
    UTC_Time utc_time;
    AV_Status status;
    Latitude latitude;
    // North/South
    Longitude longitude;
    // East/West
    Speed_Knots speed;
    TrackAngle angle;
    NMEA_Date date;
    // Magnetic variation
    // Magnetic variation East/West
    PosMode pos_mode;
    NavStatus nav_status;
} NMEA_RMC;


//--------------------------------------------------
void parse_nmea_rmc(NMEA_RMC *rmc, ASCII_DATA *data);


//--------------------------------------------------
// Logging
void log_nmea_rmc(FILE *log_fd, NMEA_RMC *rmc);
