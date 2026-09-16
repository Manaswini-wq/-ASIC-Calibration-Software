#ifndef HAL_UART_H
#define HAL_UART_H

#include "hal_types.h"

typedef struct {
    uint8_t       port_id;
    uint32_t      baud_rate;
    uart_parity_t parity;
    bool          initialized;
} hal_uart_handle_t;

hal_status_t hal_uart_init(hal_uart_handle_t *handle, uint8_t port_id,
                           uint32_t baud_rate, uart_parity_t parity);
hal_status_t hal_uart_deinit(hal_uart_handle_t *handle);
hal_status_t hal_uart_transmit(hal_uart_handle_t *handle, const uint8_t *data, size_t len);
hal_status_t hal_uart_receive(hal_uart_handle_t *handle, uint8_t *data,
                              size_t len, uint32_t timeout_ms);

#endif
