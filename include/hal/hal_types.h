#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    HAL_OK          =  0,
    HAL_ERROR       = -1,
    HAL_BUSY        = -2,
    HAL_TIMEOUT     = -3,
    HAL_INVALID_ARG = -4
} hal_status_t;

typedef enum {
    I2C_SPEED_STANDARD = 100000,
    I2C_SPEED_FAST     = 400000
} i2c_speed_t;

typedef enum {
    SPI_MODE_0 = 0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} spi_mode_t;

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
} uart_parity_t;

typedef enum {
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT
} gpio_dir_t;

typedef enum {
    GPIO_LOW  = 0,
    GPIO_HIGH = 1
} gpio_level_t;

#endif
