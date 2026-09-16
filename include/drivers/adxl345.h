/**
 * @file adxl345.h
 * @brief ADXL345 3-axis accelerometer driver (SPI interface).
 *
 * Datasheet reference: Analog Devices ADXL345.
 */
#ifndef ADXL345_H
#define ADXL345_H

#include "hal/hal_spi.h"

#define ADXL345_DEVICE_ID         0xE5

/* Register addresses */
#define ADXL345_REG_DEVID         0x00
#define ADXL345_REG_BW_RATE       0x2C
#define ADXL345_REG_POWER_CTL     0x2D
#define ADXL345_REG_DATA_FORMAT   0x31
#define ADXL345_REG_DATAX0        0x32
#define ADXL345_REG_DATAX1        0x33
#define ADXL345_REG_DATAY0        0x34
#define ADXL345_REG_DATAY1        0x35
#define ADXL345_REG_DATAZ0        0x36
#define ADXL345_REG_DATAZ1        0x37

/* Scale factor: 3.9 mg/LSB at default +/-2g range */
#define ADXL345_SCALE_FACTOR      0.0039f

typedef enum {
    ADXL345_RANGE_2G  = 0x00,
    ADXL345_RANGE_4G  = 0x01,
    ADXL345_RANGE_8G  = 0x02,
    ADXL345_RANGE_16G = 0x03
} adxl345_range_t;

typedef struct {
    float x_g;
    float y_g;
    float z_g;
} adxl345_data_t;

typedef struct {
    hal_spi_handle_t *spi;
    uint8_t           cs_pin;
    adxl345_range_t   range;
    bool              initialized;
} adxl345_handle_t;

hal_status_t adxl345_init(adxl345_handle_t *dev, hal_spi_handle_t *spi,
                          uint8_t cs_pin, adxl345_range_t range);
hal_status_t adxl345_read(adxl345_handle_t *dev, adxl345_data_t *data);

#endif
