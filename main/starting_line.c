#include <freertos/FreeRTOS.h>

#include "starting_line.h"
#include "base_button.h"


const Coordinate RESET_COORDINATE = {};
StartingLine starting_line = {};

const uint8_t GPIO_COMMITTEE_BOAT = 5;
const uint8_t GPIO_PIN_END = 6;

const uint32_t BUTTON_IDLE_DELAY = 30;
const uint32_t BUTTON_PRESSED_DELAY = 500;

const uint8_t NUM_AVG_ELEMENTS = 4;

const char TAG_STARTINGLINE[] = "[Startingline]";
const char TAG_COMMITTEE_BOAT[] = "(Committee Boat)";
const char TAG_PIN_END[] = "(Pin End)";


//--------------------------------------------------
void init_starting_line(){
    init_button(GPIO_COMMITTEE_BOAT);
    init_button(GPIO_PIN_END);
}


//--------------------------------------------------
void set_starting_line(Coordinate *starting_side, uint8_t gpio_starting_side, const char tag_starting_side[]){
    while (TRUE){
        vTaskDelay(pdMS_TO_TICKS(BUTTON_IDLE_DELAY));

        uint8_t is_pressed = gpio_get_level(gpio_starting_side);
        if (is_pressed == FALSE){ continue; }

        ESP_LOGI(TAG_STARTINGLINE, "%s: Button pressed", tag_starting_side);
        *starting_side = get_average_coordinate(&tracking_ring_buffer, NUM_AVG_ELEMENTS);
        log_starting_line(stdout, &starting_line);

        vTaskDelay(pdMS_TO_TICKS(BUTTON_PRESSED_DELAY));
        if (starting_side->is_valid == TRUE){ continue; }

        ESP_LOGW(TAG_STARTINGLINE, "%s: Invalid average coordinate", tag_starting_side);
        *starting_side = RESET_COORDINATE;
    }
}


void set_committee_boat_task(void *arg){
    ESP_LOGI(TAG_STARTINGLINE, "%s: Start Task", TAG_COMMITTEE_BOAT);
    set_starting_line(&starting_line.committee_boat, GPIO_COMMITTEE_BOAT, TAG_COMMITTEE_BOAT);
}


void set_pin_end_task(void *arg){
    ESP_LOGI(TAG_STARTINGLINE, "%s: Start Task", TAG_PIN_END);
    set_starting_line(&starting_line.pin_end, GPIO_PIN_END, TAG_PIN_END);
}


//--------------------------------------------------
// Logging
//--------------------------------------------------
void log_starting_line(FILE *log_fd, StartingLine *starting_line){
    fprintf(log_fd, "\n\nStarting Line:\n");

    fprintf(log_fd, "\n\t%s\n", TAG_COMMITTEE_BOAT);
    log_coordinate(log_fd, &starting_line->committee_boat);

    fprintf(log_fd, "\n\t%s\n", TAG_PIN_END);
    log_coordinate(log_fd, &starting_line->pin_end);
}
