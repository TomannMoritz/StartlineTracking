#include <esp_log.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "spi_display.h"


static const char TAG_DISPLAY[] = "[Display]";

// SPI PIN LAYOUT
const uint8_t GPIO_MOSI = 9;    // Master Out Slave In (MOSI)
const uint8_t GPIO_CLK = 10;    // Serial Clock (CLK)
const uint8_t GPIO_CS = 11;     // Chip Select (CS)
const uint8_t GPIO_DC = 12;     // Data/Command (DC)
const uint8_t GPIO_RST = 13;    // Reset (RST)

const int SPI_HOST = SPI2_HOST;

spi_device_handle_t spi;
u8g2_t u8g2;

const uint8_t BITS_IN_BYTE = 8;


//--------------------------------------------------
const uint32_t SCREEN_HEIGHT = 128;
const uint32_t SCREEN_WIDTH = 128;

const uint32_t CLOCK_FREQUENCE = 1 << 23;

U8G2_FONT font_small = {
    .font_height = 10,
    .font_width = 7,
    .font = u8g2_font_7x14B_mf
};

U8G2_FONT font_big = {
    .font_height = 16,
    .font_width = 14,
    .font = u8g2_font_inb16_mr
};


//--------------------------------------------------
static void init_spi(){
    spi_bus_config_t spi_bus_cfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = GPIO_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };
    spi_bus_initialize(SPI_HOST, &spi_bus_cfg, SPI_DMA_CH_AUTO);


    spi_device_interface_config_t spi_interface_cfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 1,
        .clock_speed_hz = CLOCK_FREQUENCE,
    };
    spi_bus_add_device(SPI_HOST, &spi_interface_cfg, &spi);

    gpio_set_direction(GPIO_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_RST, GPIO_MODE_OUTPUT);
}


static uint8_t u8g2_byte_cb(struct u8x8_struct *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    switch(msg){
        case U8X8_MSG_BYTE_INIT:
            init_spi();
            break;

        case U8X8_MSG_BYTE_SEND:
            spi_transaction_t transaction = {
                .length = arg_int * BITS_IN_BYTE,
                .tx_buffer = arg_ptr
            };

            spi_device_transmit(spi, &transaction);
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            break;

        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_SetDC(u8x8, arg_int);
            break;

        default:
            ESP_LOGW(TAG_DISPLAY, "U8G2 Byte CB - MSG: %d", msg);
            return 0;
    }

    return 1;
}


static uint8_t u8g2_gpio_and_delay_cb(struct u8x8_struct *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    switch(msg){
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            break;

        case U8X8_MSG_DELAY_MILLI:
            esp_rom_delay_us(arg_int * 10);
            break;

        case U8X8_MSG_GPIO_CS:
            gpio_set_level(GPIO_CS, arg_int);
            break;

        case U8X8_MSG_GPIO_DC:
            gpio_set_level(GPIO_DC, arg_int);
            break;

        case U8X8_MSG_GPIO_RESET:
            gpio_set_level(GPIO_RST, arg_int);
            break;

        default:
            ESP_LOGW(TAG_DISPLAY, "U8G2 GPIO and Delay CB - MSG: %d", msg);
            return 0;
    }

    return 1;
}


void init_u8g2(){
    ESP_LOGI(TAG_DISPLAY, "Initialize U8G2");
    u8g2_Setup_ssd1327_ea_w128128_f(&u8g2, U8G2_R0, u8g2_byte_cb, u8g2_gpio_and_delay_cb);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    ESP_LOGI(TAG_DISPLAY, "Clear Screen");
    u8g2_ClearDisplay(&u8g2);
}


//--------------------------------------------------
void display_text(char *buffer, uint32_t pos_x, uint32_t pos_y, U8G2_FONT *font, char *tag_logging){
    if (font->font_height <= 0 || font->font_width <= 0){
        ESP_LOGW(TAG_DISPLAY, "Invalid font - %s", tag_logging);
        return;
    }

    ESP_LOGI(TAG_DISPLAY, "Update Text - %s", tag_logging);
    u8g2_SetFont(&u8g2, font->font);
    u8g2_DrawStr(&u8g2, pos_x, pos_y, buffer);
    u8g2_SendBuffer(&u8g2);
}
