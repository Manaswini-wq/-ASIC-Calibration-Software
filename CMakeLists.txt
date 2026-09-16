cmake_minimum_required(VERSION 3.14)
project(embedded_sensor_hub C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler warnings — treat code like production firmware
add_compile_options(-Wall -Wextra -Werror -pedantic)

# Simulation flag — desktop build uses simulated HAL
add_definitions(-DSIMULATION_BUILD)

include_directories(${CMAKE_SOURCE_DIR}/include)

# --- Core library (everything except main) ---
set(HAL_SOURCES
    src/hal/hal_i2c.c
    src/hal/hal_spi.c
    src/hal/hal_uart.c
    src/hal/hal_gpio.c
    src/hal/hal_timer.c
)
set(DRIVER_SOURCES
    src/drivers/bme280.c
    src/drivers/adxl345.c
)
set(APP_SOURCES
    src/app/sensor_manager.c
    src/app/data_processor.c
    src/app/alert_system.c
)
set(UTIL_SOURCES
    src/utils/ring_buffer.c
    src/utils/logger.c
)

add_library(sensor_hub_lib STATIC
    ${HAL_SOURCES}
    ${DRIVER_SOURCES}
    ${APP_SOURCES}
    ${UTIL_SOURCES}
)

# --- Main application ---
add_executable(sensor_hub src/main.c)
target_link_libraries(sensor_hub sensor_hub_lib)

# --- Tests ---
enable_testing()

add_executable(test_all
    tests/test_main.c
    tests/test_ring_buffer.c
    tests/test_data_processor.c
    tests/test_bme280.c
    tests/test_adxl345.c
    tests/test_alert_system.c
)
target_link_libraries(test_all sensor_hub_lib)
add_test(NAME unit_tests COMMAND test_all)
