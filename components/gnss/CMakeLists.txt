idf_component_register(
    SRCS 
        "gnss.c"
        "nmea_rmc.c"
        "base_types.c"
    INCLUDE_DIRS
        "."
    REQUIRES
        esp_driver_uart
    )

