#include <esp_log.h>

#include "ui.h"
#include "spi_display.h"

#include "countdown.h"
#include "ring_buffer.h"


static const char *TAG_UI = "[UI]";
const uint32_t DISPLAY_DELY = 10;

const uint32_t BASE_10_000 = 10000;
const uint32_t BASE_1000 = 1000;
const uint32_t BASE_100 = 100;


//--------------------------------------------------
void init_display(){
    init_u8g2();
}


//--------------------------------------------------
const uint8_t UTC_TIME_INFO_LEN = 8;
void set_utc_time(UTC_Time *utc_time, char *buffer){
    if (utc_time->is_valid == FALSE){
        sprintf(buffer, "--:--:--");
        return;
    }

    sprintf(buffer, "%d%d:%d%d:%d%d",
            utc_time->hours / BASE_10 % BASE_10,
            utc_time->hours % BASE_10,
            utc_time->minutes / BASE_10 % BASE_10,
            utc_time->minutes % BASE_10,
            utc_time->seconds / BASE_10 % BASE_10,
            utc_time->seconds % BASE_10);
}


const uint8_t GNSS_INFO_LEN = 6;
void set_gnss_info(Coordinate *coordinate, char *buffer){
    const char *FORMAT_INFO = "GNSS";
    if (coordinate->is_valid == FALSE){
        sprintf(buffer, "%s:N", FORMAT_INFO);
        return;
    }

    sprintf(buffer, "%s:Y", FORMAT_INFO);
}


const uint8_t COUNTDOWN_INFO_LEN = 5;
void set_countdown(CountdownTimer *countdown, char *buffer){
    int8_t sign;
    uint32_t value;
    sign_of_i32(countdown->seconds, &sign, &value);

    sprintf(buffer, "%d%d:%d%d",
            (int)(value / SEC_A_MIN / BASE_10 % BASE_10),
            (int)(value / SEC_A_MIN % BASE_10),
            (int)(value % SEC_A_MIN / BASE_10),
            (int)(value % SEC_A_MIN % BASE_10));
}


const uint8_t SPEED_INFO_LEN = 7;
void set_speed(Speed_Knots *speed, char *buffer){
    if (speed->is_valid == FALSE){
        sprintf(buffer, "--.--kn");
        return;
    }

    sprintf(buffer, "%d%d.%d%dkn",
            (int)(speed->integer_value / BASE_10 % BASE_10),
            (int)(speed->integer_value % BASE_10),
            (int)(speed->decimal_value / BASE_10_000 % BASE_10),
            (int)(speed->decimal_value % BASE_10_000 / BASE_1000 % BASE_10));
}


const uint8_t ANGLE_INFO_LEN = 9;
void set_angle(TrackAngle *angle, char *buffer){
    if (angle->is_valid == FALSE){
        sprintf(buffer, "---.--Deg");
        return;
    }

    sprintf(buffer, "%d%d%d.%d%dDeg",
            (int)(angle->integer_value / BASE_100 % BASE_10),
            (int)(angle->integer_value / BASE_10 % BASE_10),
            (int)(angle->integer_value % BASE_10),
            (int)(angle->decimal_value / BASE_10 % BASE_10),
            (int)(angle->decimal_value % BASE_10));
}


void display_task(void *){
    ESP_LOGI(TAG_UI, "Start Display Task");

    while (true){
        ESP_LOGI(TAG_UI, "Update Display");

        // TrackingData (GNSS)
        TrackingData curr_data = tracking_ring_buffer.tracking_data[tracking_ring_buffer.start_position];

        // UTC Time
        char utc_time_buffer[UTC_TIME_INFO_LEN + NULL_TERMINATOR_BYTES];
        set_utc_time(&curr_data.utc_time, utc_time_buffer);
        display_text(utc_time_buffer, font_small.font_width, font_small.font_height, &font_small, "UTC Time");

        // GNSS Info
        char gnss_info_buffer[GNSS_INFO_LEN + NULL_TERMINATOR_BYTES];
        set_gnss_info(&curr_data.coordinate, gnss_info_buffer);
        display_text(gnss_info_buffer, SCREEN_WIDTH - font_small.font_width * GNSS_INFO_LEN, font_small.font_height, &font_small, "GNSS Info");

        // Countdown
        char countdown_buffer[COUNTDOWN_INFO_LEN + NULL_TERMINATOR_BYTES];
        set_countdown(&countdown_timer, countdown_buffer);
        display_text(countdown_buffer, SCREEN_WIDTH / 2 - (COUNTDOWN_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT / 4, &font_big, "Countdown");


        // Speed
        char speed_buffer[SPEED_INFO_LEN + NULL_TERMINATOR_BYTES];
        set_speed(&curr_data.speed, speed_buffer);
        display_text(speed_buffer, SCREEN_WIDTH / 2 - (SPEED_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT / 2, &font_big, "Speed");

        // Angle
        char angle_buffer[ANGLE_INFO_LEN + NULL_TERMINATOR_BYTES];
        set_angle(&curr_data.angle, angle_buffer);
        display_text(angle_buffer, SCREEN_WIDTH / 2 - (ANGLE_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT * 3 / 4, &font_big, "Angle");

        vTaskDelay(pdMS_TO_TICKS(DISPLAY_DELY));
    }
}
