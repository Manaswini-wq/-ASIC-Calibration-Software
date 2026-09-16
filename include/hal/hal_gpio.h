#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "hal_types.h"

#define GPIO_MAX_PINS 32

hal_status_t hal_gpio_init(uint8_t pin, gpio_dir_t direction);
hal_status_t hal_gpio_write(uint8_t pin, gpio_level_t level);
gpio_level_t hal_gpio_read(uint8_t pin);
hal_status_t hal_gpio_toggle(uint8_t pin);

#endif
