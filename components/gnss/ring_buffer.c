#include <math.h>

#include "ring_buffer.h"

#include "vector.h"
#include "constants.h"

RingBuffer tracking_ring_buffer;


//--------------------------------------------------
void create_coordinate(Coordinate *coordinate, Latitude *latitude, Longitude *longitude){
    coordinate->is_valid = latitude->is_valid && longitude->is_valid;

    coordinate->latitude = *latitude;
    coordinate->longitude = *longitude;
}


void create_speed_angle(SpeedAngle *speed_angle, SpeedKnots *speed, TrackAngle *angle){
    speed_angle->is_valid = speed->is_valid && angle->is_valid;

    speed_angle->speed = *speed;
    speed_angle->angle = *angle;
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
    Coordinate invalid_coordinate = {};
    if (length > RING_BUFFER_SIZE){ return invalid_coordinate; }
    // Note: with i64 it is unlikely to exceed the max number of elements
    if (length > RING_BUFFER_MAX_AVG_ELEMENTS_I64){ return invalid_coordinate; }

    int64_t latitude_value = 0;
    int64_t longitude_value = 0;
    uint32_t counter_valid = 0;

    for (size_t i = 0; i < length; i++){
        uint32_t position = (ring_buffer->start_position + i) % RING_BUFFER_SIZE;

        Coordinate coordinate = ring_buffer->tracking_data[position].coordinate;
        if (coordinate.is_valid == FALSE){ continue; }

        latitude_value += (int64_t)latitude_to_number(&coordinate.latitude);
        longitude_value += (int64_t)longitude_to_number(&coordinate.longitude);
        counter_valid++;
    }

    if (counter_valid == 0){ return invalid_coordinate; }

    latitude_value /= counter_valid;
    longitude_value /= counter_valid;

    Coordinate avg_coordinate = {
        .is_valid = TRUE,
        .latitude = number_to_latitude((int32_t)latitude_value),
        .longitude = number_to_longitude((int32_t)longitude_value)
    };
    return avg_coordinate;
}


SpeedAngle get_average_speed_angle(RingBuffer *ring_buffer, uint32_t length){
    SpeedAngle invalid_speed_angle = {};
    if (length > RING_BUFFER_SIZE){ return invalid_speed_angle; }
    // Note: with i64 it is unlikely to exceed the max number of elements
    if (length > RING_BUFFER_MAX_AVG_ELEMENTS_I64){ return invalid_speed_angle; }

    uint32_t counter_valid = 0;
    Vector2 vector = {};

    for (size_t i = 0; i < length; i++){
        uint32_t position = (ring_buffer->start_position + i) % RING_BUFFER_SIZE;

        TrackingData curr_data = ring_buffer->tracking_data[position];
        if (curr_data.speed_angle.is_valid == FALSE){ continue; }

        // add direction vectors (scaled by speed) to calculate average over both parameters
        float compass_deg = curr_data.speed_angle.angle.value;
        float speed_value = curr_data.speed_angle.speed.value;

        Vector2 curr_vector = compass_to_vector2(compass_deg);
        scale_vector2(&curr_vector, speed_value);
        add_vector2(&vector, &curr_vector);

        counter_valid++;
    }

    if (counter_valid == 0){ return invalid_speed_angle; }

    Vector2 avg_vector = vector;
    scale_vector2(&avg_vector, 1.0 / counter_valid);

    float avg_speed = vector2_length(&avg_vector);
    scale_vector2(&avg_vector, 1.0 / avg_speed);

    float avg_compass = vector2_to_compass(avg_vector);

    SpeedAngle avg_speed_angle = {
        .is_valid = TRUE,
        .angle.is_valid = TRUE,
        .angle.value = avg_compass,
        .speed.is_valid = TRUE,
        .speed.value = avg_speed
    };
    return avg_speed_angle;
}


void calculate_average_tracking_data(RingBuffer *ring_buffer, uint32_t length, TrackingData *tracking_data){
    if (length > RING_BUFFER_SIZE){ return; }

    // Note: Store the last UTC Time within the specified range/length. This makes it possible to calculate
    // the elapsed time when combined with the UTC Time of the newest Tracking Data element.
    uint32_t position = (ring_buffer->start_position + length) % RING_BUFFER_SIZE;
    tracking_data->utc_time = ring_buffer->tracking_data[position].utc_time;

    tracking_data->coordinate = get_average_coordinate(ring_buffer, length);
    tracking_data->speed_angle = get_average_speed_angle(ring_buffer, length);
}


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_coordinate(FILE *log_fd, Coordinate *coordinate){
    LOG_VALUE_u8(log_fd, coordinate->is_valid);
    log_latitude(log_fd, &coordinate->latitude);
    log_longitude(log_fd, &coordinate->longitude);
}


void log_speed_angle(FILE *log_fd, SpeedAngle *speed_angle){
    LOG_VALUE_u8(log_fd, speed_angle->is_valid);
    log_speed_knots(log_fd, &speed_angle->speed);
    log_track_angle(log_fd, &speed_angle->angle);
}


void log_tracking_data(FILE *log_fd, TrackingData *tracking_data){
    log_utc_time(log_fd, &tracking_data->utc_time);

    log_coordinate(log_fd, &tracking_data->coordinate);
    log_speed_angle(log_fd, &tracking_data->speed_angle);
}


void log_ring_buffer(FILE *log_fd, RingBuffer *ring_buffer){
    for (size_t i = 0; i < RING_BUFFER_SIZE; i++){
        uint32_t position = (ring_buffer->start_position + i) % RING_BUFFER_SIZE;

        fprintf(log_fd, "\n\nElement: %zu - Position: %u\n", i, (unsigned int)position);
        log_tracking_data(log_fd, &ring_buffer->tracking_data[position]);
    }
}
