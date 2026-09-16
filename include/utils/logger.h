/**
 * @file logger.h
 * @brief Lightweight logging over UART with severity levels.
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "hal/hal_uart.h"

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

void logger_init(hal_uart_handle_t *uart, log_level_t min_level);
void logger_log(log_level_t level, const char *module, const char *fmt, ...);

#define LOG_D(mod, ...) logger_log(LOG_DEBUG, mod, __VA_ARGS__)
#define LOG_I(mod, ...) logger_log(LOG_INFO,  mod, __VA_ARGS__)
#define LOG_W(mod, ...) logger_log(LOG_WARN,  mod, __VA_ARGS__)
#define LOG_E(mod, ...) logger_log(LOG_ERROR, mod, __VA_ARGS__)

#endif
