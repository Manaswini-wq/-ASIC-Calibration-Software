/**
 * @file hal_spi.c
 * @brief Simulated SPI HAL — register-based memory model.
 */
#include "hal/hal_spi.h"
#include <string.h>

#define SPI_REG_MAP_SIZE 64

static uint8_t sim_spi_regs[SPI_REG_MAP_SIZE];
static bool spi_sim_initialized = false;

void hal_spi_sim_set_reg(uint8_t reg, uint8_t value) {
    if (reg < SPI_REG_MAP_SIZE) sim_spi_regs[reg] = value;
}

void hal_spi_sim_reset(void) {
    memset(sim_spi_regs, 0, sizeof(sim_spi_regs));
    spi_sim_initialized = false;
}

hal_status_t hal_spi_init(hal_spi_handle_t *handle, uint8_t bus_id,
                          uint32_t clock_hz, spi_mode_t mode) {
    if (!handle) return HAL_INVALID_ARG;
    handle->bus_id = bus_id;
    handle->clock_hz = clock_hz;
    handle->mode = mode;
    handle->initialized = true;
    spi_sim_initialized = true;
    return HAL_OK;
}

hal_status_t hal_spi_deinit(hal_spi_handle_t *handle) {
    if (!handle) return HAL_INVALID_ARG;
    handle->initialized = false;
    return HAL_OK;
}

hal_status_t hal_spi_transfer(hal_spi_handle_t *handle, uint8_t cs_pin,
                              const uint8_t *tx, uint8_t *rx, size_t len) {
    (void)cs_pin;
    if (!handle || !handle->initialized) return HAL_ERROR;
    if (tx && rx) {
        for (size_t i = 0; i < len; i++) {
            uint8_t addr = tx[i] & 0x3F;
            rx[i] = (addr < SPI_REG_MAP_SIZE) ? sim_spi_regs[addr] : 0;
        }
    }
    return HAL_OK;
}

hal_status_t hal_spi_read_reg(hal_spi_handle_t *handle, uint8_t cs_pin,
                              uint8_t reg_addr, uint8_t *data, size_t len) {
    (void)cs_pin;
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;
    for (size_t i = 0; i < len; i++) {
        uint8_t addr = (reg_addr + (uint8_t)i) & 0x3F;
        data[i] = (addr < SPI_REG_MAP_SIZE) ? sim_spi_regs[addr] : 0;
    }
    return HAL_OK;
}

hal_status_t hal_spi_write_reg(hal_spi_handle_t *handle, uint8_t cs_pin,
                               uint8_t reg_addr, const uint8_t *data, size_t len) {
    (void)cs_pin;
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;
    for (size_t i = 0; i < len; i++) {
        uint8_t addr = (reg_addr + (uint8_t)i) & 0x3F;
        if (addr < SPI_REG_MAP_SIZE) sim_spi_regs[addr] = data[i];
    }
    return HAL_OK;
}
