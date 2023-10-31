set(sources
    ${API_SRCS}
    "main.c"
    "drivers/pn532/pn532.c"
    "drivers/pn532/pn532_user.c"
    # "drivers/vl53l0x/vl53l0x.c"
    # "drivers/vl53l0x/vl53l0x_user.c"
    "drivers/i2c_driver/i2c_driver.c"
    "drivers/vl53l0x_2/core/src/vl53l0x_api_calibration.c"
    # "drivers/vl53l0x_2/core/src/vl53l0x_api_core.c"
    # "drivers/vl53l0x_2/core/src/vl53l0x_api_ranging.c"
    # "drivers/vl53l0x_2/core/src/vl53l0x_api_strings.c"
    # "drivers/vl53l0x_2/core/src/vl53l0x_api.c"
    # "drivers/vl53l0x_2/platform/src/vl53l0x_platform_log.c"
    # "drivers/vl53l0x_2/platform/src/vl53l0x_platform.c"    
    # "drivers/vl53l0x_2/vl53l0x_platform_log.c"
    # "drivers/vl53l0x_2/vl53l0x_platform.c"
    # "drivers/vl53l0x_2/vl53l0x.c"

)

set(includes
    ${API_INCLUDES}
    "."
    "drivers/i2c_driver/"
    # "drivers/vl53l0x"
    "drivers/vl53l0x_2/core/inc"
    "drivers/vl53l0x_2/platform/inc"
    "drivers/pn532"
)

idf_component_register(SRCS ${sources}
                    INCLUDE_DIRS ${includes})

