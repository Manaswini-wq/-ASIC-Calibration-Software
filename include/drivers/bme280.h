/**
 * @file bme280.h
 * @brief BME280 Temperature/Humidity/Pressure sensor driver (I2C).
 *
 * Datasheet reference: Bosch BME280 — registers 0xF7-0xFE for raw data,
 * 0x88-0xA1 / 0xE1-0xE7 for calibration/compensation parameters.
 */
#ifndef BME280_H
#define BME280_H

#include "hal/hal_i2c.h"

#define BME280_I2C_ADDR_PRIMARY   0x76
#define BME280_I2C_ADDR_SECONDARY 0x77

#define BME280_CHIP_ID            0x60

/* Key registers */
#define BME280_REG_CHIP_ID        0xD0
#define BME280_REG_RESET          0xE0
#define BME280_REG_CTRL_HUM       0xF2
#define BME280_REG_STATUS         0xF3
#define BME280_REG_CTRL_MEAS      0xF4
#define BME280_REG_CONFIG         0xF5
#define BME280_REG_DATA_START     0xF7

typedef enum {
    BME280_OVERSAMPLE_1  = 1,
    BME280_OVERSAMPLE_2  = 2,
    BME280_OVERSAMPLE_4  = 3,
    BME280_OVERSAMPLE_8  = 4,
    BME280_OVERSAMPLE_16 = 5
} bme280_oversample_t;

typedef struct {
    /* Temperature compensation */
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    /* Pressure compensation */
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    /* Humidity compensation */
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bme280_calib_t;

typedef struct {
    float temperature_c;
    float pressure_pa;
    float humidity_pct;
} bme280_data_t;

typedef struct {
    hal_i2c_handle_t *i2c;
    uint8_t           addr;
    bme280_calib_t    calib;
    int32_t           t_fine;   /* shared between temp/pressure compensation */
    bool              initialized;
} bme280_handle_t;

hal_status_t bme280_init(bme280_handle_t *dev, hal_i2c_handle_t *i2c, uint8_t addr);
hal_status_t bme280_read(bme280_handle_t *dev, bme280_data_t *data);
hal_status_t bme280_soft_reset(bme280_handle_t *dev);

#endif
