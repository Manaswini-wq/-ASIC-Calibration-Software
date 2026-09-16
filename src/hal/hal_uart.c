/**
 * @file hal_uart.c
 * @brief Simulated UART HAL — prints to stdout on desktop.
 */
#include "hal/hal_uart.h"
#include <stdio.h>
#include <string.h>

#define UART_RX_BUF_SIZE 256

static uint8_t rx_buffer[UART_RX_BUF_SIZE];
static size_t  rx_len = 0;

hal_status_t hal_uart_init(hal_uart_handle_t *handle, uint8_t port_id,
                           uint32_t baud_rate, uart_parity_t parity) {
    if (!handle) return HAL_INVALID_ARG;
    handle->port_id = port_id;
    handle->baud_rate = baud_rate;
    handle->parity = parity;
    handle->initialized = true;
    return HAL_OK;
}

hal_status_t hal_uart_deinit(hal_uart_handle_t *handle) {
    if (!handle) return HAL_INVALID_ARG;
    handle->initialized = false;
    return HAL_OK;
}

hal_status_t hal_uart_transmit(hal_uart_handle_t *handle, const uint8_t *data, size_t len) {
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;

#ifdef SIMULATION_BUILD
    fwrite(data, 1, len, stdout);
    fflush(stdout);
#endif
    return HAL_OK;
}

hal_status_t hal_uart_receive(hal_uart_handle_t *handle, uint8_t *data,
                              size_t len, uint32_t timeout_ms) {
    (void)timeout_ms;
    if (!handle || !data) return HAL_INVALID_ARG;
    if (!handle->initialized) return HAL_ERROR;

    size_t copy = (len < rx_len) ? len : rx_len;
    if (copy > 0) {
        memcpy(data, rx_buffer, copy);
        rx_len -= copy;
        memmove(rx_buffer, rx_buffer + copy, rx_len);
    }
    return HAL_OK;
}
