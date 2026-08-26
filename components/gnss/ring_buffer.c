#include "ring_buffer.h"


//--------------------------------------------------
void create_coordinate(Coordinate *coordinate, Latitude *latitude, Longitude *longitude){
    coordinate->is_valid = latitude->is_valid && longitude->is_valid;

    coordinate->latitude = *latitude;
    coordinate->longitude = *longitude;
}


void ring_buffer_insert(RingBuffer *ring_buffer, TrackingData *tracking_data){
    if (ring_buffer->start_position == 0){
        ring_buffer->start_position = RING_BUFFER_SIZE;
    }
    ring_buffer->start_position--;

    ring_buffer->tracking_data[ring_buffer->start_position] = *tracking_data;
}


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_coordinate(FILE *log_fd, Coordinate *coordinate){
    LOG_VALUE_u8(log_fd, coordinate->is_valid);
    log_latitude(log_fd, &coordinate->latitude);
    log_longitude(log_fd, &coordinate->longitude);
}


void log_tracking_data(FILE *log_fd, TrackingData *tracking_data){
    log_utc_time(log_fd, &tracking_data->utc_time);

    log_coordinate(log_fd, &tracking_data->coordinate);

    log_speed_knots(log_fd, &tracking_data->speed);
    log_track_angle(log_fd, &tracking_data->angle);
}


void log_ring_buffer(FILE *log_fd, RingBuffer *ring_buffer){
    for (size_t i = 0; i < RING_BUFFER_SIZE; i++){
        uint32_t position = (ring_buffer->start_position + i) % RING_BUFFER_SIZE;

        fprintf(log_fd, "\n\nElement: %zu - Position: %u\n", i, position);
        log_tracking_data(log_fd, &ring_buffer->tracking_data[position]);
    }
}
