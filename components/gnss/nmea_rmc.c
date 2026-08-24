#include "nmea_rmc.h"

const char NMEA_RMC_MSG[SSS_BYTES + NULL_TERMINATOR_BYTES] = "RMC";


//--------------------------------------------------
void parse_nmea_rmc(NMEA_RMC *rmc, ASCII_DATA *data){
    parse_utc_time(&rmc->utc_time, data);
    parse_av_status(&rmc->status, data);
    parse_latitude(&rmc->latitude, data);
    parse_longitude(&rmc->longitude, data);
    parse_speed_knots(&rmc->speed, data);
    parse_track_angle(&rmc->angle, data);
    parse_nmea_date(&rmc->date, data);
    parse_magnetic_variation(data);
    parse_position_mode(&rmc->pos_mode, data);
    parse_nav_status(&rmc->nav_status, data);
}


//--------------------------------------------------
void log_nmea_rmc(FILE *log_fd, NMEA_RMC *rmc){
    log_utc_time(stdout, &rmc->utc_time);
    log_av_status(stdout, &rmc->status);
    log_latitude(stdout, &rmc->latitude);
    log_longitude(stdout, &rmc->longitude);
    log_speed_knots(stdout, &rmc->speed);
    log_track_angle(stdout, &rmc->angle);
    log_nmea_date(stdout, &rmc->date);
    log_position_mode(stdout, &rmc->pos_mode);
    log_nav_status(stdout, &rmc->nav_status);
}
