cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(yama_2_GFX_SSD1306_graphictest_spi_project)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(
       yama_2_GFX_SSD1306_graphictest_spi   yama_2_GFX_SSD1306_graphictest_spi.cpp
                                            yama_2_GFX.cpp
                                            yama_2_GFX_SSD1306.cpp
)

target_link_libraries(yama_2_GFX_SSD1306_graphictest_spi
                       pico_stdlib
                       hardware_spi
                       hardware_i2c
)

pico_enable_stdio_usb(yama_2_GFX_SSD1306_graphictest_spi 1)
pico_enable_stdio_uart(yama_2_GFX_SSD1306_graphictest_spi 0)

pico_add_extra_outputs(yama_2_GFX_SSD1306_graphictest_spi)

