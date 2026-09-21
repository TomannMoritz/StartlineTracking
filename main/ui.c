#include <esp_log.h>

#include "ui.h"
#include "spi_display.h"

#include "countdown.h"
#include "ring_buffer.h"
#include "gps_util.h"

#include "constants.h"


static const char *TAG_UI = "[UI]";
const uint32_t DISPLAY_DELAY = 10;
const uint32_t HEADLINE_OFFSET = 8;

enum DISPLAY_VIEW_STATE { PRE_START, POST_START };
enum DISPLAY_VIEW_STATE curr_view = PRE_START;


//--------------------------------------------------
void init_display(){
    init_u8g2();
}


//--------------------------------------------------
enum { UTC_TIME_INFO_LEN = 8 };
char utc_time_buffer[UTC_TIME_INFO_LEN + NULL_TERMINATOR_BYTES];

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


enum { GNSS_INFO_LEN = 6 };
char gnss_info_buffer[GNSS_INFO_LEN + NULL_TERMINATOR_BYTES];

void set_gnss_info(Coordinate *coordinate, char *buffer){
    const char *FORMAT_INFO = "GNSS";
    if (coordinate->is_valid == FALSE){
        sprintf(buffer, "%s:N", FORMAT_INFO);
        return;
    }

    sprintf(buffer, "%s:Y", FORMAT_INFO);
}


enum { COUNTDOWN_INFO_LEN = 5 };
char countdown_buffer[COUNTDOWN_INFO_LEN + NULL_TERMINATOR_BYTES];

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


enum { SPEED_INFO_LEN = 7 };
char speed_buffer[SPEED_INFO_LEN + NULL_TERMINATOR_BYTES];

void set_speed(SpeedKnots *speed, char *buffer){
    if (speed->is_valid == FALSE){
        sprintf(buffer, "--.--kn");
        return;
    }

    int integer_value = (int)speed->value;
    int decimal_value = (int)(speed->value * BASE_100) % BASE_100;

    sprintf(buffer, "%d%d.%d%dkn",
            (int)(integer_value / BASE_10 % BASE_10),
            (int)(integer_value % BASE_10),
            (int)(decimal_value / BASE_10 % BASE_10),
            (int)(decimal_value % BASE_10));
}


enum { ANGLE_INFO_LEN = 9 };
char angle_buffer[ANGLE_INFO_LEN + NULL_TERMINATOR_BYTES];

void set_angle(TrackAngle *angle, char *buffer){
    if (angle->is_valid == FALSE){
        sprintf(buffer, "---.--Deg");
        return;
    }

    int integer_value = (int)angle->value;
    int decimal_value = (int)(angle->value * BASE_100) % BASE_100;
    sprintf(buffer, "%d%d%d.%d%dDeg",
            (int)(integer_value / BASE_100 % BASE_10),
            (int)(integer_value / BASE_10 % BASE_10),
            (int)(integer_value % BASE_10),
            (int)(decimal_value / BASE_10 % BASE_10),
            (int)(decimal_value % BASE_10));
}


//--------------------------------------------------
void display_pre_start(){
    ESP_LOGI(TAG_UI, "Update Display (Pre Start)");

    // Countdown
    set_countdown(&countdown_timer, countdown_buffer);
    display_text(countdown_buffer, SCREEN_WIDTH / 2 - (COUNTDOWN_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT / 4 + HEADLINE_OFFSET, &font_big, "Countdown");
}


void display_post_start(TrackingData *curr_data){
    ESP_LOGI(TAG_UI, "Update Display (Post Start)");

    // Speed
    set_speed(&curr_data->speed_angle.speed, speed_buffer);
    display_text(speed_buffer, SCREEN_WIDTH / 2 - (SPEED_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT / 4 + HEADLINE_OFFSET, &font_big, "Speed");

    // Angle
    set_angle(&curr_data->speed_angle.angle, angle_buffer);
    display_text(angle_buffer, SCREEN_WIDTH / 2 - (ANGLE_INFO_LEN * font_big.font_width) / 2, SCREEN_HEIGHT / 2 + HEADLINE_OFFSET, &font_big, "Angle");

    // Countdown
    set_countdown(&countdown_timer, countdown_buffer);
    display_text(countdown_buffer, SCREEN_WIDTH / 2 - (COUNTDOWN_INFO_LEN * font_small.font_width) / 2, SCREEN_HEIGHT * 3 / 4, &font_small, "Countdown");
}


//--------------------------------------------------
void display_task(void *){
    ESP_LOGI(TAG_UI, "Start Display Task");

    while (true){
        ESP_LOGI(TAG_UI, "Update Display");
        if (countdown_timer.seconds <= 0){
            curr_view = PRE_START;
        }

        // TrackingData (GNSS)
        TrackingData curr_data = tracking_ring_buffer.tracking_data[tracking_ring_buffer.start_position];

        // UTC Time
        set_utc_time(&curr_data.utc_time, utc_time_buffer);
        display_text(utc_time_buffer, font_small.font_width, font_small.font_height, &font_small, "UTC Time");

        // GNSS Info
        set_gnss_info(&curr_data.coordinate, gnss_info_buffer);
        display_text(gnss_info_buffer, SCREEN_WIDTH - font_small.font_width * GNSS_INFO_LEN, font_small.font_height, &font_small, "GNSS Info");

        // NOTE: clear display when switching beteen views
        switch (curr_view){
            case PRE_START:
                display_pre_start();

                if (countdown_timer.seconds > 0){
                    curr_view = POST_START;
                    display_clear("PRE START");
                }
                break;

            case POST_START:
                display_post_start(&curr_data);
                break;

            default:
                ESP_LOGE(TAG_UI, "Invalid View State");
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(DISPLAY_DELAY));
    }
}
