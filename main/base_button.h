#pragma once
#include <stdint.h>
#include <driver/gpio.h>

extern const uint32_t BUTTON_IDLE_DELAY;
extern const uint32_t BUTTON_PRESSED_DELAY;

typedef struct {
    const char *log_tag_component;
    const char *log_tag_function;
    void (*function)(void);
    const uint8_t gpio_pin;
} ButtonFunction;


//--------------------------------------------------
void init_button(uint8_t pin_number);
void button_task(void *pv_parameters);
