set(sources
    ${API_SRCS}
    "main.c"
    "app/msm/msm.c"
    "drivers/pn532/pn532.c"
    "drivers/pn532/pn532_user.c"
    "drivers/cap_touch/cap_touch.c"
    "drivers/ws2812/ws2812.c"
    "drivers/ws2812/ws2812_api.c"
    "drivers/i2c_driver/i2c_driver.c"
    "drivers/button/button.c"
    "drivers/button/button_api.c"

)

set(includes
    ${API_INCLUDES}
    "."
    "app/msm"
    "drivers/i2c_driver/"
    "drivers/pn532"
    "drivers/cap_touch"
    "drivers/ws2812"
    "drivers/button"
)

idf_component_register(SRCS ${sources}
                    INCLUDE_DIRS ${includes})

