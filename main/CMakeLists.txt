idf_component_register(SRCS "main.cpp" "fp_serial.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_uart esp_driver_gpio esp_timer)
