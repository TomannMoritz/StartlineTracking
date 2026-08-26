#include "ring_buffer.h"

RingBuffer tracking_ring_buffer;


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
Coordinate get_average_coordinate(RingBuffer *ring_buffer, uint32_t length){
    Coordinate avg_coordinate = {};
    if (length > RING_BUFFER_SIZE){ return avg_coordinate; }
    // Note: with i64 it is unlikely to exceed the max number of elements
    if (length > RING_BUFFER_MAX_AVG_ELEMENTS_I64){ return avg_coordinate; }

    int64_t latitude_value = 0;
    int64_t longitude_value = 0;
    uint32_t counter_coordinate = 0;

    for (size_t i = 0; i < length; i++){
        uint32_t position = (ring_buffer->start_position + i) % RING_BUFFER_SIZE;

        Coordinate coordinate = ring_buffer->tracking_data[position].coordinate;
        if (coordinate.is_valid == FALSE){ continue; }

        latitude_value += (int64_t)latitude_to_number(&coordinate.latitude);
        longitude_value += (int64_t)longitude_to_number(&coordinate.longitude);
        counter_coordinate++;
    }

    if (counter_coordinate == 0){ return avg_coordinate; }

    latitude_value /= counter_coordinate;
    longitude_value /= counter_coordinate;

    avg_coordinate.is_valid = TRUE;
    avg_coordinate.latitude = number_to_latitude((int32_t)latitude_value);
    avg_coordinate.longitude = number_to_longitude((int32_t)longitude_value);

    return avg_coordinate;
}


void calculate_average_tracking_data(RingBuffer *ring_buffer, uint32_t length, TrackingData *tracking_data){
    if (length > RING_BUFFER_SIZE){ return; }

    // Note: Store the last UTC Time within the specified range/length. This makes it possible to calculate
    // the elapsed time when combined with the UTC Time of the newest Tracking Data element.
    uint32_t position = (ring_buffer->start_position + length) % RING_BUFFER_SIZE;
    tracking_data->utc_time = ring_buffer->tracking_data[position].utc_time;

    tracking_data->coordinate = get_average_coordinate(ring_buffer, length);

    // TODO: calculate speed & angle average
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

        fprintf(log_fd, "\n\nElement: %zu - Position: %u\n", i, (unsigned int)position);
        log_tracking_data(log_fd, &ring_buffer->tracking_data[position]);
    }
}
