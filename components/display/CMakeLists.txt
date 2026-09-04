idf_component_register(
    SRCS 
        "spi_display.c"
    INCLUDE_DIRS
        "."
    REQUIRES
        esp_driver_spi
        esp_driver_gpio
        "u8g2"
    )

