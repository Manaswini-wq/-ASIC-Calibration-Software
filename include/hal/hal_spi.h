#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "hal_types.h"

typedef struct {
    uint8_t    bus_id;
    uint32_t   clock_hz;
    spi_mode_t mode;
    bool       initialized;
} hal_spi_handle_t;

hal_status_t hal_spi_init(hal_spi_handle_t *handle, uint8_t bus_id,
                          uint32_t clock_hz, spi_mode_t mode);
hal_status_t hal_spi_deinit(hal_spi_handle_t *handle);
hal_status_t hal_spi_transfer(hal_spi_handle_t *handle, uint8_t cs_pin,
                              const uint8_t *tx, uint8_t *rx, size_t len);
hal_status_t hal_spi_read_reg(hal_spi_handle_t *handle, uint8_t cs_pin,
                              uint8_t reg_addr, uint8_t *data, size_t len);
hal_status_t hal_spi_write_reg(hal_spi_handle_t *handle, uint8_t cs_pin,
                               uint8_t reg_addr, const uint8_t *data, size_t len);

#endif
