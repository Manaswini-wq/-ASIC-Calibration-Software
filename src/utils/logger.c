#include "utils/logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static hal_uart_handle_t *log_uart = NULL;
static log_level_t min_log_level = LOG_INFO;

static const char *level_str[] = { "DBG", "INF", "WRN", "ERR" };

void logger_init(hal_uart_handle_t *uart, log_level_t min_level) {
    log_uart = uart;
    min_log_level = min_level;
}

void logger_log(log_level_t level, const char *module, const char *fmt, ...) {
    if (level < min_log_level) return;

    char buf[256];
    int offset = snprintf(buf, sizeof(buf), "[%s][%s] ", level_str[level], module);

    va_list args;
    va_start(args, fmt);
    offset += vsnprintf(buf + offset, sizeof(buf) - (size_t)offset, fmt, args);
    va_end(args);

    if ((size_t)offset < sizeof(buf) - 1) {
        buf[offset++] = '\n';
    }

    if (log_uart && log_uart->initialized) {
        hal_uart_transmit(log_uart, (const uint8_t *)buf, (size_t)offset);
    }
}
