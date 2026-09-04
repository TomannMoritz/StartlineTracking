
#include <freertos/FreeRTOS.h>

#include "gnss.h"
#include "starting_line.h"
#include "countdown.h"
#include "ui.h"

#define STACK_DEPTH (1024 * 4)

//--------------------------------------------------
void init(){
    init_gnss_urat();
    init_starting_line();
    init_countdown();
    init_display();
}


void start_tasks(){
    xTaskCreate(gnss_rx_task, "GNSS UART RX", STACK_DEPTH, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(set_committee_boat_task, "Committe Boat", STACK_DEPTH, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(set_pin_end_task, "Pin End", STACK_DEPTH, NULL, configMAX_PRIORITIES - 1, NULL);

    start_countdown();
    xTaskCreate(display_task, "Display", STACK_DEPTH, NULL, configMAX_PRIORITIES - 1, NULL);
}


//--------------------------------------------------
void app_main(){
    init();
    start_tasks();
}
