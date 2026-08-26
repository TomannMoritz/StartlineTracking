
#pragma once
#include "gnss.h"

//--------------------------------------------------
typedef struct {
    uint8_t is_valid;
    Latitude latitude;
    Longitude longitude;
} Coordinate;


typedef struct {
    UTC_Time utc_time;
    Coordinate coordinate;
    Speed_Knots speed;
    TrackAngle angle;
} TrackingData;


//--------------------------------------------------
enum { RING_BUFFER_SIZE = 16 };
typedef struct {
    TrackingData tracking_data[RING_BUFFER_SIZE];
    uint32_t start_position;
} RingBuffer;


//--------------------------------------------------
void create_coordinate(Coordinate *coordinate, Latitude *latitude, Longitude *longitude);
void ring_buffer_insert(RingBuffer *ring_buffer, TrackingData *track_data);


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_coordinate(FILE *log_fd, Coordinate *coordinate);
void log_tracking_data(FILE *log_fd, TrackingData *tracking_data);
void log_ring_buffer(FILE *log_fd, RingBuffer *ring_buffer);
