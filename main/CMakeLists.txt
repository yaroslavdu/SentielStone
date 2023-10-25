set(sources
    ${API_SRCS}
    "main.c"
    "drivers/pn532/pn532.c"
    "drivers/pn532/pn532_user.c"
    "drivers/vl53l0x/vl53l0x.c"
    "drivers/vl53l0x/vl53l0x_user.c"
)

set(includes
    ${API_INCLUDES}
    "."
    "drivers/vl53l0x"
    "drivers/pn532"
)

idf_component_register(SRCS ${sources}
                    INCLUDE_DIRS ${includes})

# idf_component_register(SRCS "main.c"
#                     INCLUDE_DIRS ".")
