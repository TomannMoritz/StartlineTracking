#pragma once
#include <stdint.h>
#include <stdio.h>

#define LOG_VALUE_u8(fd, x) fprintf(fd, "\t%s = %d\n", #x, x);
#define LOG_VALUE_u32(fd, x) fprintf(fd, "\t%s = %ld\n", #x, x);
#define LOG_VALUE_ASCII(fd, x) fprintf(fd, "\t%s = %c\n", #x, x);


#define SKIP_NMEA_MSG(data) while(data->data[data->offset] != ASCII_START){ data->offset++; }
#define SKIP_FIELD(data) while(data->data[data->offset] != ASCII_SEPARATOR){ data->offset++; }
#define RETURN_EMPTY_FIELD(data) if (data->data[data->offset] == ASCII_SEPARATOR){ data->offset++; return; }


extern const uint8_t ASCII_START;
extern const uint8_t ASCII_SEPARATOR;
extern const uint8_t ASCII_ZERO;
extern const uint8_t BASE_10;


//--------------------------------------------------
typedef struct {
    uint8_t *data;
    uint32_t offset;
} ASCII_DATA;


// Note: sign specifies the hemisphere for Latitude/Longitude
typedef struct {
    int8_t value;
} Sign;


//--------------------------------------------------
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t miliseconds;
} UTC_Time;


extern const uint8_t ACTIVE_STATUS;
extern const uint8_t VOID_STATUS;
typedef struct {
    uint8_t value;
} AV_Status;


// Note: sign specifies the northern/southern hemisphere
// (0, 90]: North
// [-90,0): South
extern const int8_t NORTH_CHAR;
extern const int8_t SOUTH_CHAR;
typedef struct {
    Sign sign;
    uint8_t degrees;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t miliseconds;
} Latitude;


// Note: sign specifies the eastern/western hemisphere
// (0, 180]: East
// [-180, 0): West
extern const int8_t EAST_CHAR;
extern const int8_t WEST_CHAR;
typedef struct {
    Sign sign;
    uint8_t degrees;
    uint8_t minutes;
    uint8_t seconds;
    uint32_t miliseconds;
} Longitude;


typedef struct {
    uint8_t integer_value;
    uint32_t decimal_value;
} Speed_Knots;


typedef struct {
    uint8_t integer_value;
    uint8_t decimal_value;
} TrackAngle;


typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t year;
} NMEA_Date;


// Note: mv and mvEW are not defined/used
typedef struct {
} MagneticVariation;


typedef struct {
    uint8_t value;
} PosMode;


typedef struct {
    uint8_t value;
} NavStatus;


//----------------------------------------------------------------------------------------------------
void parse_utc_time(UTC_Time *utc_time, ASCII_DATA *data);
void parse_av_status(AV_Status *status, ASCII_DATA *data);
void parse_latitude(Latitude *latitude, ASCII_DATA *data);
void parse_longitude(Longitude *longitude, ASCII_DATA *data);
void parse_speed_knots(Speed_Knots *speed, ASCII_DATA *data);
void parse_track_angle(TrackAngle *angle, ASCII_DATA *data);
void parse_nmea_date(NMEA_Date *date, ASCII_DATA *data);
void parse_magnetic_variation(ASCII_DATA *data);
void parse_position_mode(PosMode *mode, ASCII_DATA *data);
void parse_nav_status(NavStatus *nav_status, ASCII_DATA *data);


//----------------------------------------------------------------------------------------------------
// Logging
//----------------------------------------------------------------------------------------------------
void log_utc_time(FILE *log_fd, UTC_Time *time);
void log_av_status(FILE *log_fd, AV_Status *status);
void log_latitude(FILE *log_fd, Latitude *latitude);
void log_longitude(FILE *log_fd, Longitude *longitude);
void log_speed_knots(FILE *log_fd, Speed_Knots *speed);
void log_track_angle(FILE *log_fd, TrackAngle *angle);
void log_nmea_date(FILE *log_fd, NMEA_Date *date);
void log_position_mode(FILE *log_fd, PosMode *pos_mode);
void log_nav_status(FILE *log_fd, NavStatus *nav_status);
