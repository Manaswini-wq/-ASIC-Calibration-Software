#ifndef HAL_I2C_H
#define HAL_I2C_H

#include "hal_types.h"

typedef struct {
    uint8_t     bus_id;
    i2c_speed_t speed;
    bool        initialized;
} hal_i2c_handle_t;

hal_status_t hal_i2c_init(hal_i2c_handle_t *handle, uint8_t bus_id, i2c_speed_t speed);
hal_status_t hal_i2c_deinit(hal_i2c_handle_t *handle);
hal_status_t hal_i2c_read_reg(hal_i2c_handle_t *handle, uint8_t dev_addr,
                              uint8_t reg_addr, uint8_t *data, size_t len);
hal_status_t hal_i2c_write_reg(hal_i2c_handle_t *handle, uint8_t dev_addr,
                               uint8_t reg_addr, const uint8_t *data, size_t len);

#endif
