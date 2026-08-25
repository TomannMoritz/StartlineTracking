#include "base_types.h"

const uint8_t ASCII_START = '$';
const uint8_t ASCII_SEPARATOR = ',';
const uint8_t ASCII_ZERO = '0';
const uint8_t BASE_10 = 10;

const uint8_t ACTIVE_STATUS = 'A';
const uint8_t VOID_STATUS = 'V';

const int8_t NORTH_CHAR = 'N';
const int8_t SOUTH_CHAR = 'S';

const int8_t EAST_CHAR = 'E';
const int8_t WEST_CHAR = 'W';


//--------------------------------------------------
uint8_t ascii_to_number(uint8_t value){
    return value - ASCII_ZERO;
}


void sign_of_i32(int32_t number, int8_t *sign, uint32_t *value){
    uint8_t is_positive = number >= 0;
    *sign = (is_positive) ? 1 : -1;

    *value = (uint32_t)(number * *sign);
}


const uint32_t MIN_IN_DEG = 60;
const uint32_t SEC_IN_MIN = 60;
const uint32_t MIL_IN_SEC = 1000;

const uint32_t DEG_SCALE = MIN_IN_DEG * SEC_IN_MIN * MIL_IN_SEC;
const uint32_t MIN_SCALE = SEC_IN_MIN * MIL_IN_SEC;
const uint32_t SEC_SCALE = MIL_IN_SEC;


int32_t geographical_position_to_number(GeographicalPosition *position){
    int32_t result = 0
        + position->degrees * DEG_SCALE
        + position->minutes * MIN_SCALE
        + position->seconds * SEC_SCALE
        + position->miliseconds;

    return result * position->sign;
}


GeographicalPosition number_to_geographical_position(int32_t number){
    int8_t sign;
    uint32_t value;
    sign_of_i32(number, &sign, &value);

    uint8_t deg =  (value - 0) / (DEG_SCALE);
    uint8_t min =  (value - deg * DEG_SCALE) / (MIN_SCALE);
    uint8_t sec =  (value - deg * DEG_SCALE - min * MIN_SCALE) / (SEC_SCALE);
    uint32_t mil = (value - deg * DEG_SCALE - min * MIN_SCALE - sec * SEC_SCALE);

    GeographicalPosition position = {
        .is_valid = TRUE,
        .sign = sign,
        .degrees = deg,
        .minutes = min,
        .seconds = sec,
        .miliseconds = mil
    };

    return position;
}


Latitude number_to_latitude(int32_t number){
    GeographicalPosition position = number_to_geographical_position(number);

    Latitude latitude = {
        .is_valid = position.is_valid,
        .sign.value = position.sign,
        .degrees = position.degrees,
        .minutes = position.minutes,
        .seconds = position.seconds,
        .miliseconds = position.miliseconds
    };

    return latitude;
}


Longitude number_to_longitude(int32_t number){
    GeographicalPosition position = number_to_geographical_position(number);

    Longitude longitude = {
        .is_valid = position.is_valid,
        .sign.value = position.sign,
        .degrees = position.degrees,
        .minutes = position.minutes,
        .seconds = position.seconds,
        .miliseconds = position.miliseconds
    };

    return longitude;
}


uint32_t latitude_to_number(Latitude *latitude){
    GeographicalPosition position = {
        .sign = latitude->sign.value,
        .degrees = latitude->degrees,
        .minutes = latitude->minutes,
        .seconds = latitude->seconds,
        .miliseconds = latitude->miliseconds
    };

    return geographical_position_to_number(&position);
}


uint32_t longitude_to_number(Longitude *longitude){
    GeographicalPosition position = {
        .sign = longitude->sign.value,
        .degrees = longitude->degrees,
        .minutes = longitude->minutes,
        .seconds = longitude->seconds,
        .miliseconds = longitude->miliseconds
    };

    return geographical_position_to_number(&position);
}


//--------------------------------------------------
void parse_sign(Sign *sign, ASCII_DATA *data){
    uint8_t value = data->data[data->offset];
    if (value == NORTH_CHAR || value == EAST_CHAR){
            sign->value = 1;
            data->offset++;
    }

    if (value == SOUTH_CHAR || value == WEST_CHAR){
            sign->value = -1;
            data->offset++;
    }

    data->offset++;
}


//--------------------------------------------------
void parse_talker_identifier(TalkerIdentifier *talker, ASCII_DATA *data){
    talker->value[0] = data->data[data->offset];
    talker->value[1] = data->data[data->offset + 1];
    talker->value[2] = NULL_TERMINATOR;

    data->offset += TT_BYTES;
}


void parse_sentence_formater(SentenceFormater *formater, ASCII_DATA *data){

    formater->value[0] = data->data[data->offset];
    formater->value[1] = data->data[data->offset + 1];
    formater->value[2] = data->data[data->offset + 2];
    formater->value[3] = NULL_TERMINATOR;

    data->offset += SSS_BYTES;
}


//--------------------------------------------------
void parse_utc_time(UTC_Time *utc_time, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    utc_time->hours = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    utc_time->hours += ascii_to_number(data->data[data->offset]);
    data->offset++;

    utc_time->minutes = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    utc_time->minutes += ascii_to_number(data->data[data->offset]);
    data->offset++;

    utc_time->seconds = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    utc_time->seconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    // separator at position 6
    data->offset++;

    utc_time->miliseconds = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    utc_time->miliseconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    utc_time->is_valid = TRUE;
    data->offset++;
}


void parse_av_status(AV_Status *status, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    status->value = data->data[data->offset];
    data->offset++;

    status->is_valid = TRUE;
    data->offset++;
}


// Note: Latitude/Longitude are defined by two fields (coordinates, sign/hemisphere)
// -> parse both fields into one datatype
void parse_latitude(Latitude *latitude, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    latitude->degrees = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    latitude->degrees += ascii_to_number(data->data[data->offset]);
    data->offset++;

    latitude->minutes = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    latitude->minutes += ascii_to_number(data->data[data->offset]);
    data->offset++;

    // separator at position 4
    data->offset++;
    
    latitude->seconds = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    latitude->seconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    latitude->miliseconds = ascii_to_number(data->data[data->offset]) * BASE_10 * BASE_10;
    data->offset++;
    latitude->miliseconds += ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    latitude->miliseconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    latitude->is_valid = TRUE;
    data->offset++;

    // parse NORTH/SOUTH sign/hemisphere
    RETURN_EMPTY_FIELD(data);
    parse_sign(&latitude->sign, data);
}


void parse_longitude(Longitude *longitude, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    longitude->degrees = ascii_to_number(data->data[data->offset]) * BASE_10 * BASE_10;
    data->offset++;
    longitude->degrees += ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    longitude->degrees += ascii_to_number(data->data[data->offset]);
    data->offset++;

    longitude->minutes = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    longitude->minutes += ascii_to_number(data->data[data->offset]);
    data->offset++;

    // separator at position 5
    data->offset++;
    
    longitude->seconds = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    longitude->seconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    longitude->miliseconds = ascii_to_number(data->data[data->offset]) * BASE_10 * BASE_10;
    data->offset++;
    longitude->miliseconds += ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    longitude->miliseconds += ascii_to_number(data->data[data->offset]);
    data->offset++;

    longitude->is_valid = TRUE;
    data->offset++;

    // parse EAST/WEST sign/hemisphere
    RETURN_EMPTY_FIELD(data);
    parse_sign(&longitude->sign, data);
}


void parse_speed_knots(Speed_Knots *speed, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    speed->decimal_value = ascii_to_number(data->data[data->offset]);
    data->offset++;

    // separator at position 1
    data->offset++;

    speed->decimal_value = ascii_to_number(data->data[data->offset]) * BASE_10 * BASE_10;
    data->offset++;
    speed->decimal_value += ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    speed->decimal_value += ascii_to_number(data->data[data->offset]);
    data->offset++;

    speed->is_valid = TRUE;
    data->offset++;
}


void parse_track_angle(TrackAngle *angle, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    angle->integer_value = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    angle->integer_value += ascii_to_number(data->data[data->offset]);
    data->offset++;

    // separator at position 2
    data->offset++;

    angle->integer_value = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    angle->integer_value += ascii_to_number(data->data[data->offset]);
    data->offset++;

    angle->is_valid = TRUE;
    data->offset++;
}


void parse_nmea_date(NMEA_Date *date, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    date->day = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    date->day += ascii_to_number(data->data[data->offset]);
    data->offset++;

    date->month = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    date->month += ascii_to_number(data->data[data->offset]);
    data->offset++;

    date->year = ascii_to_number(data->data[data->offset]) * BASE_10;
    data->offset++;
    date->year += ascii_to_number(data->data[data->offset]);
    data->offset++;

    date->is_valid = TRUE;
    data->offset++;
}


void parse_magnetic_variation(ASCII_DATA *data){
    // skip magnetic variation
    SKIP_FIELD(data);
    data->offset++;

    // skip magnetic variation east/west
    SKIP_FIELD(data);
    data->offset++;
}


void parse_position_mode(PosMode *mode, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    mode->value = data->data[data->offset];
    data->offset++;

    mode->is_valid = TRUE;
    data->offset++;
}


void parse_nav_status(NavStatus *nav_status, ASCII_DATA *data){
    RETURN_EMPTY_FIELD(data);

    nav_status->value = data->data[data->offset];
    data->offset++;

    nav_status->is_valid = TRUE;
    data->offset++;
}


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_utc_time(FILE *log_fd, UTC_Time *time){
    LOG_VALUE_u8(log_fd, time->is_valid);
    LOG_VALUE_u8(log_fd, time->hours);
    LOG_VALUE_u8(log_fd, time->minutes);
    LOG_VALUE_u8(log_fd, time->seconds);
    LOG_VALUE_u8(log_fd, time->miliseconds);
}

void log_av_status(FILE *log_fd, AV_Status *status){
    LOG_VALUE_u8(log_fd, status->is_valid);
    LOG_VALUE_ASCII(log_fd, status->value);
}

void log_latitude(FILE *log_fd, Latitude *latitude){
    LOG_VALUE_u8(log_fd, latitude->is_valid);
    LOG_VALUE_u8(log_fd, latitude->sign.value);
    LOG_VALUE_u8(log_fd, latitude->degrees);
    LOG_VALUE_u8(log_fd, latitude->minutes);
    LOG_VALUE_u8(log_fd, latitude->seconds);
    LOG_VALUE_u32(log_fd, latitude->miliseconds);
}

void log_longitude(FILE *log_fd, Longitude *longitude){
    LOG_VALUE_u8(log_fd, longitude->is_valid);
    LOG_VALUE_u8(log_fd, longitude->sign.value);
    LOG_VALUE_u8(log_fd, longitude->degrees);
    LOG_VALUE_u8(log_fd, longitude->minutes);
    LOG_VALUE_u8(log_fd, longitude->seconds);
    LOG_VALUE_u32(log_fd, longitude->miliseconds);
}

void log_speed_knots(FILE *log_fd, Speed_Knots *speed){
    LOG_VALUE_u8(log_fd, speed->is_valid);
    LOG_VALUE_u8(log_fd, speed->integer_value);
    LOG_VALUE_u32(log_fd, speed->decimal_value);
}

void log_track_angle(FILE *log_fd, TrackAngle *angle){
    LOG_VALUE_u8(log_fd, angle->is_valid);
    LOG_VALUE_u8(log_fd, angle->integer_value);
    LOG_VALUE_u8(log_fd, angle->decimal_value);
}

void log_nmea_date(FILE *log_fd, NMEA_Date *date){
    LOG_VALUE_u8(log_fd, date->is_valid);
    LOG_VALUE_u8(log_fd, date->day);
    LOG_VALUE_u8(log_fd, date->month);
    LOG_VALUE_u8(log_fd, date->year);
}

void log_position_mode(FILE *log_fd, PosMode *pos_mode){
    LOG_VALUE_u8(log_fd, pos_mode->is_valid);
    LOG_VALUE_ASCII(log_fd, pos_mode->value);
}

void log_nav_status(FILE *log_fd, NavStatus *nav_status){
    LOG_VALUE_u8(log_fd, nav_status->is_valid);
    LOG_VALUE_ASCII(log_fd, nav_status->value);
}

