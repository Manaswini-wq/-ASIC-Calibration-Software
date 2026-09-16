/**
 * @file hal_gpio.c
 * @brief Simulated GPIO HAL.
 */
#include "hal/hal_gpio.h"

static gpio_level_t pin_states[GPIO_MAX_PINS];
static gpio_dir_t   pin_dirs[GPIO_MAX_PINS];
static bool         pin_inited[GPIO_MAX_PINS];

hal_status_t hal_gpio_init(uint8_t pin, gpio_dir_t direction) {
    if (pin >= GPIO_MAX_PINS) return HAL_INVALID_ARG;
    pin_dirs[pin] = direction;
    pin_states[pin] = GPIO_LOW;
    pin_inited[pin] = true;
    return HAL_OK;
}

hal_status_t hal_gpio_write(uint8_t pin, gpio_level_t level) {
    if (pin >= GPIO_MAX_PINS || !pin_inited[pin]) return HAL_INVALID_ARG;
    if (pin_dirs[pin] != GPIO_DIR_OUTPUT) return HAL_ERROR;
    pin_states[pin] = level;
    return HAL_OK;
}

gpio_level_t hal_gpio_read(uint8_t pin) {
    if (pin >= GPIO_MAX_PINS || !pin_inited[pin]) return GPIO_LOW;
    return pin_states[pin];
}

hal_status_t hal_gpio_toggle(uint8_t pin) {
    if (pin >= GPIO_MAX_PINS || !pin_inited[pin]) return HAL_INVALID_ARG;
    if (pin_dirs[pin] != GPIO_DIR_OUTPUT) return HAL_ERROR;
    pin_states[pin] = (pin_states[pin] == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
    return HAL_OK;
}
