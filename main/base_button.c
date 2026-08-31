#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#include "base_button.h"
#include "base_types.h"


const uint32_t BUTTON_IDLE_DELAY = 30;
const uint32_t BUTTON_PRESSED_DELAY = 500;


void init_button(uint8_t pin_number){
    const gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << pin_number),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&button_config);
}


//--------------------------------------------------
void button_task(void *pv_parameters){
    ButtonFunction *button_function = (ButtonFunction *)pv_parameters;
    ESP_LOGI(button_function->log_tag_component, "%s: Start Task", button_function->log_tag_function);

    while(TRUE){
        vTaskDelay(pdMS_TO_TICKS(BUTTON_IDLE_DELAY));

        uint8_t is_pressed = gpio_get_level(button_function->gpio_pin);
        if (is_pressed == FALSE){ continue; }

        ESP_LOGI(button_function->log_tag_component, "%s: Button Pressed", button_function->log_tag_function);
        (button_function->function)();

        vTaskDelay(pdMS_TO_TICKS(BUTTON_PRESSED_DELAY));
    }
}

