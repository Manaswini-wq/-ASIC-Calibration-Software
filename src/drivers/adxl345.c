/**
 * @file adxl345.c
 * @brief ADXL345 accelerometer driver over SPI.
 */
#include "drivers/adxl345.h"

static float get_scale(adxl345_range_t range) {
    switch (range) {
        case ADXL345_RANGE_4G:  return ADXL345_SCALE_FACTOR * 2.0f;
        case ADXL345_RANGE_8G:  return ADXL345_SCALE_FACTOR * 4.0f;
        case ADXL345_RANGE_16G: return ADXL345_SCALE_FACTOR * 8.0f;
        default:                return ADXL345_SCALE_FACTOR;
    }
}

hal_status_t adxl345_init(adxl345_handle_t *dev, hal_spi_handle_t *spi,
                          uint8_t cs_pin, adxl345_range_t range) {
    if (!dev || !spi) return HAL_INVALID_ARG;

    dev->spi = spi;
    dev->cs_pin = cs_pin;
    dev->range = range;
    dev->initialized = false;

    /* Verify device ID */
    uint8_t id = 0;
    hal_status_t status = hal_spi_read_reg(spi, cs_pin, ADXL345_REG_DEVID, &id, 1);
    if (status != HAL_OK) return status;
    if (id != ADXL345_DEVICE_ID) return HAL_ERROR;

    /* Set data format (range + full resolution) */
    uint8_t fmt = 0x08 | (range & 0x03);
    status = hal_spi_write_reg(spi, cs_pin, ADXL345_REG_DATA_FORMAT, &fmt, 1);
    if (status != HAL_OK) return status;

    /* Set 100 Hz output data rate */
    uint8_t rate = 0x0A;
    status = hal_spi_write_reg(spi, cs_pin, ADXL345_REG_BW_RATE, &rate, 1);
    if (status != HAL_OK) return status;

    /* Enter measurement mode */
    uint8_t pwr = 0x08;
    status = hal_spi_write_reg(spi, cs_pin, ADXL345_REG_POWER_CTL, &pwr, 1);
    if (status != HAL_OK) return status;

    dev->initialized = true;
    return HAL_OK;
}

hal_status_t adxl345_read(adxl345_handle_t *dev, adxl345_data_t *data) {
    if (!dev || !data) return HAL_INVALID_ARG;
    if (!dev->initialized) return HAL_ERROR;

    uint8_t raw[6];
    hal_status_t status = hal_spi_read_reg(dev->spi, dev->cs_pin,
                                           ADXL345_REG_DATAX0, raw, 6);
    if (status != HAL_OK) return status;

    int16_t raw_x = (int16_t)(raw[1] << 8 | raw[0]);
    int16_t raw_y = (int16_t)(raw[3] << 8 | raw[2]);
    int16_t raw_z = (int16_t)(raw[5] << 8 | raw[4]);

    float scale = get_scale(dev->range);
    data->x_g = raw_x * scale;
    data->y_g = raw_y * scale;
    data->z_g = raw_z * scale;

    return HAL_OK;
}
