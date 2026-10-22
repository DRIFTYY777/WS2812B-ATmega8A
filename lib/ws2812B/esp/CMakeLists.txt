idf_component_register(SRCS "esp_ws2812b.c" "led_strip_encoder.h"
                       PRIV_REQUIRES esp_driver_rmt
                       INCLUDE_DIRS ".")
