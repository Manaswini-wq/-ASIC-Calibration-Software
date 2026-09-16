/**
 * @file hal_i2c.c
 * @brief Simulated I2C HAL — stores register maps in memory for desktop testing.
 *        On real MCU, replace with STM32 HAL / ESP-IDF I2C calls.
 */
#include "hal/hal_i2c.h"
#include <string.h>

#define MAX_I2C_DEVICES 8
#define REG_MAP_SIZE    256

typedef struct {
    uint8_t addr;
    uint8_t regs[REG_MAP_SIZE];
    bool    active;
} sim_i2c_device_t;

static sim_i2c_device_t sim_devices[MAX_I2C_DEVICES];
static bool sim_initialized = false;

/* Exposed for test injection */
void hal_i2c_sim_add_device(uint8_t addr, const uint8_t *initial_regs, size_t len) {
    for (int i = 0; i < MAX_I2C_DEVICES; i++) {
        if (!sim_devices[i].active) {
            sim_devices[i].addr = addr;
            sim_devices[i].active = true;
            memset(sim_devices[i].regs, 0, REG_MAP_SIZE);
            if (initial_regs && len > 0) {
                size_t copy_len = len < REG_MAP_SIZE ? len : REG_MAP_SIZE;
                memcpy(sim_devices[i].regs, initial_regs, copy_len);
            }
            return;
        }
    }
}

void hal_i2c_sim_reset(void) {
    memset(sim_devices, 0, sizeof(sim_devices));
    sim_initialized = false;
}

static sim_i2c_device_t *find_device(uint8_t addr) {
    for (int i = 0; i < MAX_I2C_DEVICES; i++) {
        if (sim_devices[i].active && sim_devices[i].addr == addr)
            return &sim_devices[i];
    }
    return NULL;
}

hal_status_t hal_i2c_init(hal_i2c_handle_t *handle, uint8_t bus_id, i2c_speed_t speed) {
    if (!handle) return HAL_INVALID_ARG;
    handle->bus_id = bus_id;
    handle->speed = speed;
    handle->initialized = true;
    sim_initialized = true;
    return HAL_OK;
}

hal_status_t hal_i2c_deinit(hal_i2c_handle_t *handle) {
    if (!handle) return HAL_INVALID_ARG;
    handle->initialized = false;
    return HAL_OK;
}

hal_status_t hal_i2c_read_reg(hal_i2c_handle_t *handle, uint8_t dev_addr,
                              uint8_t reg_addr, uint8_t *data, size_t len) {
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;

    sim_i2c_device_t *dev = find_device(dev_addr);
    if (!dev) return HAL_ERROR;

    for (size_t i = 0; i < len; i++) {
        data[i] = dev->regs[(reg_addr + i) & 0xFF];
    }
    return HAL_OK;
}

hal_status_t hal_i2c_write_reg(hal_i2c_handle_t *handle, uint8_t dev_addr,
                               uint8_t reg_addr, const uint8_t *data, size_t len) {
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;

    sim_i2c_device_t *dev = find_device(dev_addr);
    if (!dev) return HAL_ERROR;

    for (size_t i = 0; i < len; i++) {
        dev->regs[(reg_addr + i) & 0xFF] = data[i];
    }
    return HAL_OK;
}
