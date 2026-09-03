#include <freertos/FreeRTOS.h>
#include "esp_log.h"

#include "countdown.h"
#include "base_button.h"

#define STACK_DEPTH (1024 * 4)

const uint8_t SEC_A_MIN = 60;
static const int32_t COUNTDOWN_TIMER_MIN = -15 * SEC_A_MIN;
static const int32_t COUNTDOWN_TIMER_MAX = INT32_MAX >> 1;

static const int32_t COUNTDOWN_TIMER_SEC = -3 * SEC_A_MIN;
static const uint32_t COUNTDOWN_TIMER_RUNNING_DELAY = 100;
static const uint32_t COUNTDOWN_TIMER_STOP_DELAY = 100;

const uint8_t GPIO_COUNTDOWN_START_STOP = 3;
const uint8_t GPIO_COUNTDOWN_SYNC_CLEAR = 4;
const uint8_t GPIO_COUNTDOWN_ADD_MIN = 5;
const uint8_t GPIO_COUNTDOWN_SUB_MIN = 6;

static const char TAG_COUNTDOWN[] = "[Countdown Timer]";
static const char TAG_COUNTDOWN_START_STOP[] = "(START/STOP)";
static const char TAG_COUNTDOWN_SNYC_CLEAR[] = "(SYNC/CLEAR)";
static const char TAG_COUNTDOWN_ADD_MIN[] = "(ADD MIN)";
static const char TAG_COUNTDOWN_SUB_MIN[] = "(SUB MIN)";

CountdownTimer countdown_timer;


//--------------------------------------------------
static void init_countdown_timer(void){
    ESP_LOGI(TAG_COUNTDOWN, "Setup Countdown timer");
    countdown_timer.is_running = FALSE;
    countdown_timer.seconds = COUNTDOWN_TIMER_SEC;
}


static void init_countdown_buttons(void){
    ESP_LOGI(TAG_COUNTDOWN, "Setup Countdown buttons");
    init_button(GPIO_COUNTDOWN_START_STOP);
    init_button(GPIO_COUNTDOWN_SYNC_CLEAR);
    init_button(GPIO_COUNTDOWN_ADD_MIN);
    init_button(GPIO_COUNTDOWN_SUB_MIN);
}


void init_countdown(void){
    init_countdown_timer();
    init_countdown_buttons();
}


//--------------------------------------------------
static void bound_timer(void){
    if (countdown_timer.seconds < COUNTDOWN_TIMER_MIN){
        ESP_LOGW(TAG_COUNTDOWN, "Timer below bounds");
        countdown_timer.seconds = COUNTDOWN_TIMER_MIN;
    }

    if (countdown_timer.seconds > COUNTDOWN_TIMER_MAX){
        ESP_LOGW(TAG_COUNTDOWN, "Timer above bounds: %d - %d", countdown_timer.seconds, COUNTDOWN_TIMER_MAX);
        countdown_timer.seconds = COUNTDOWN_TIMER_MAX;
    }
}


static void countdown_start_stop_pressed(void){
    // Note: update previous tick to reset the tick count for the update task
    countdown_timer.previous_tick = xTaskGetTickCount();

    countdown_timer.is_running = ~countdown_timer.is_running;
    countdown_timer.is_running &= 0b1;
    log_countdown_timer(stdout, &countdown_timer);
}


static void countdown_sync_clear_pressed(void){
    // Note: update previous tick to reset the tick count for the update task
    countdown_timer.previous_tick = xTaskGetTickCount();

    // Note: synchronize when the timer is running OR clear when the timer is stopped
    uint8_t clearing = countdown_timer.is_running == FALSE;
    if (clearing == TRUE){
        countdown_timer.seconds = 0;

    }else{
        // Note: sync should fast forward to the next minute - if to far the timer can always be stopped
        int8_t sign;
        uint32_t value;
        sign_of_i32(countdown_timer.seconds, &sign, &value);

        countdown_timer.seconds += value % SEC_A_MIN;
    }

    log_countdown_timer(stdout, &countdown_timer);
}


static void countdown_add_min_pressed(void){
    countdown_timer.seconds -= SEC_A_MIN;
    log_countdown_timer(stdout, &countdown_timer);
}


static void countdown_sub_min_pressed(void){
    countdown_timer.seconds += SEC_A_MIN;
    log_countdown_timer(stdout, &countdown_timer);
}


static void countdown_update_task(void *){
    ESP_LOGI(TAG_COUNTDOWN, "Start Countdown Timer Task");
    countdown_timer.previous_tick = xTaskGetTickCount();

    while (TRUE){
        if (countdown_timer.is_running == FALSE){
            vTaskDelay(pdMS_TO_TICKS(COUNTDOWN_TIMER_STOP_DELAY));
            continue;
        }

        vTaskDelayUntil(&countdown_timer.previous_tick, COUNTDOWN_TIMER_RUNNING_DELAY);
        countdown_timer.seconds++;
        bound_timer();
        log_countdown_timer(stdout, &countdown_timer);
    }
}


void start_countdown(void){
    static ButtonFunction bf_start_stop = {
        .log_tag_component = TAG_COUNTDOWN,
        .log_tag_function = TAG_COUNTDOWN_START_STOP,
        .function = countdown_start_stop_pressed,
        .gpio_pin = GPIO_COUNTDOWN_START_STOP
    };

    xTaskCreate(button_task, "Countdown - Start/Stop", STACK_DEPTH, (void *)&bf_start_stop, configMAX_PRIORITIES - 1, NULL);

    static ButtonFunction bf_sync_clear = {
        .log_tag_component = TAG_COUNTDOWN,
        .log_tag_function = TAG_COUNTDOWN_SNYC_CLEAR,
        .function = countdown_sync_clear_pressed,
        .gpio_pin = GPIO_COUNTDOWN_SYNC_CLEAR
    };
    xTaskCreate(button_task, "Countdown - SYNC/CLEAR", STACK_DEPTH, &bf_sync_clear, configMAX_PRIORITIES - 1, NULL);

    static ButtonFunction bf_add_min = {
        .log_tag_component = TAG_COUNTDOWN,
        .log_tag_function = TAG_COUNTDOWN_ADD_MIN,
        .function = countdown_add_min_pressed,
        .gpio_pin = GPIO_COUNTDOWN_ADD_MIN
    };
    xTaskCreate(button_task, "Countdown - ADD MIN", STACK_DEPTH, &bf_add_min, configMAX_PRIORITIES - 1, NULL);

    static ButtonFunction bf_sub_min = {
        .log_tag_component = TAG_COUNTDOWN,
        .log_tag_function = TAG_COUNTDOWN_SUB_MIN,
        .function = countdown_sub_min_pressed,
        .gpio_pin = GPIO_COUNTDOWN_SUB_MIN
    };
    xTaskCreate(button_task, "Countdown - SUB MIN", STACK_DEPTH, &bf_sub_min, configMAX_PRIORITIES - 1, NULL);

    xTaskCreate(countdown_update_task, "Countdown - UPDATE", STACK_DEPTH, NULL, configMAX_PRIORITIES - 1, NULL);
}


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_countdown_timer(FILE *log_fd, CountdownTimer *countdown_timer){
    fprintf(log_fd, "%s\n", TAG_COUNTDOWN);
    LOG_VALUE_u8(log_fd, countdown_timer->is_running);

    LOG_VALUE_u32(log_fd, countdown_timer->seconds);
    uint32_t u_value;
    int8_t sign;
    sign_of_i32(countdown_timer->seconds, &sign, &u_value);
    unsigned int value = (unsigned int)u_value;

    fprintf(log_fd, "\t%d%d:%d%d\n\n",
            (value / SEC_A_MIN) / 10,
            (value / SEC_A_MIN) % 10,
            (value % SEC_A_MIN) / 10,
            (value % SEC_A_MIN) % 10);
}
