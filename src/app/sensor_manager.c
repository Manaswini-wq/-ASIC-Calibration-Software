#include "app/sensor_manager.h"
#include "hal/hal_timer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TEMP_HIGH_ALERT_PIN   10
#define TEMP_LOW_ALERT_PIN    11
#define HUMIDITY_HIGH_ALERT_PIN 12

hal_status_t sensor_manager_init(sensor_manager_t *mgr,
                                 hal_i2c_handle_t *i2c,
                                 hal_spi_handle_t *spi,
                                 uint8_t accel_cs_pin) {
    if (!mgr || !i2c || !spi) return HAL_INVALID_ARG;

    memset(mgr, 0, sizeof(*mgr));

    /* UART for logging */
    hal_status_t status = hal_uart_init(&mgr->uart, 0, 115200, UART_PARITY_NONE);
    if (status != HAL_OK) return status;

    logger_init(&mgr->uart, LOG_DEBUG);
    LOG_I("INIT", "Sensor Manager starting...");

    /* BME280 (I2C) */
    status = bme280_init(&mgr->bme280, i2c, BME280_I2C_ADDR_PRIMARY);
    if (status != HAL_OK) {
        LOG_E("INIT", "BME280 init failed: %d", status);
        return status;
    }
    LOG_I("INIT", "BME280 initialized");

    /* ADXL345 (SPI) */
    status = adxl345_init(&mgr->adxl345, spi, accel_cs_pin, ADXL345_RANGE_2G);
    if (status != HAL_OK) {
        LOG_E("INIT", "ADXL345 init failed: %d", status);
        return status;
    }
    LOG_I("INIT", "ADXL345 initialized");

    /* Data processors */
    moving_avg_init(&mgr->temp_avg, MOVING_AVG_WINDOW);
    moving_avg_init(&mgr->humidity_avg, MOVING_AVG_WINDOW);
    moving_avg_init(&mgr->pressure_avg, MOVING_AVG_WINDOW);

    /* Alerts */
    alert_system_init(&mgr->alerts);
    alert_system_add(&mgr->alerts, "temperature", ALERT_ABOVE, 35.0f, 2.0f,
                     TEMP_HIGH_ALERT_PIN);
    alert_system_add(&mgr->alerts, "temperature", ALERT_BELOW, 0.0f, 2.0f,
                     TEMP_LOW_ALERT_PIN);
    alert_system_add(&mgr->alerts, "humidity", ALERT_ABOVE, 80.0f, 5.0f,
                     HUMIDITY_HIGH_ALERT_PIN);

    LOG_I("INIT", "Sensor Manager ready");
    return HAL_OK;
}

hal_status_t sensor_manager_read_cycle(sensor_manager_t *mgr) {
    bme280_data_t env_data;
    hal_status_t status = bme280_read(&mgr->bme280, &env_data);
    if (status != HAL_OK) {
        mgr->error_count++;
        LOG_E("READ", "BME280 read failed");
        return status;
    }

    moving_avg_push(&mgr->temp_avg, env_data.temperature_c);
    moving_avg_push(&mgr->humidity_avg, env_data.humidity_pct);
    moving_avg_push(&mgr->pressure_avg, env_data.pressure_pa);

    alert_system_check(&mgr->alerts, "temperature", env_data.temperature_c);
    alert_system_check(&mgr->alerts, "humidity", env_data.humidity_pct);

    adxl345_data_t accel_data;
    status = adxl345_read(&mgr->adxl345, &accel_data);
    if (status != HAL_OK) {
        mgr->error_count++;
        LOG_E("READ", "ADXL345 read failed");
        return status;
    }

    mgr->sample_count++;

    LOG_D("DATA", "T=%.2fC H=%.1f%% P=%.0fPa X=%.3fg Y=%.3fg Z=%.3fg",
          (double)env_data.temperature_c, (double)env_data.humidity_pct,
          (double)env_data.pressure_pa,
          (double)accel_data.x_g, (double)accel_data.y_g, (double)accel_data.z_g);

    return HAL_OK;
}

void sensor_manager_print_report(sensor_manager_t *mgr) {
    LOG_I("REPORT", "=== Sensor Report (samples: %u, errors: %u) ===",
          mgr->sample_count, mgr->error_count);
    LOG_I("REPORT", "Temperature: avg=%.2fC min=%.2fC max=%.2fC",
          (double)moving_avg_get(&mgr->temp_avg),
          (double)moving_avg_get_min(&mgr->temp_avg),
          (double)moving_avg_get_max(&mgr->temp_avg));
    LOG_I("REPORT", "Humidity:    avg=%.1f%% min=%.1f%% max=%.1f%%",
          (double)moving_avg_get(&mgr->humidity_avg),
          (double)moving_avg_get_min(&mgr->humidity_avg),
          (double)moving_avg_get_max(&mgr->humidity_avg));
    LOG_I("REPORT", "Pressure:    avg=%.0fPa",
          (double)moving_avg_get(&mgr->pressure_avg));
}
