#include "base_button.h"


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
