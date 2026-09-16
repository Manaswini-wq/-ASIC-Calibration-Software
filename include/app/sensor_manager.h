/**
 * @file sensor_manager.h
 * @brief Coordinates sensor reading, data processing, alerting, and UART output.
 */
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "drivers/bme280.h"
#include "drivers/adxl345.h"
#include "app/data_processor.h"
#include "app/alert_system.h"
#include "utils/logger.h"

#define MOVING_AVG_WINDOW 10

typedef struct {
    bme280_handle_t   bme280;
    adxl345_handle_t  adxl345;
    moving_avg_t      temp_avg;
    moving_avg_t      humidity_avg;
    moving_avg_t      pressure_avg;
    alert_system_t    alerts;
    hal_uart_handle_t uart;
    uint32_t          sample_count;
    uint32_t          error_count;
} sensor_manager_t;

hal_status_t sensor_manager_init(sensor_manager_t *mgr,
                                 hal_i2c_handle_t *i2c,
                                 hal_spi_handle_t *spi,
                                 uint8_t accel_cs_pin);
hal_status_t sensor_manager_read_cycle(sensor_manager_t *mgr);
void         sensor_manager_print_report(sensor_manager_t *mgr);

#endif
