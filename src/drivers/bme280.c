/**
 * @file bme280.c
 * @brief BME280 driver — reads raw ADC values and applies compensation formulas
 *        from the Bosch BME280 datasheet (section 4.2.3).
 */
#include "drivers/bme280.h"
#include <string.h>

static hal_status_t read_reg(bme280_handle_t *dev, uint8_t reg, uint8_t *buf, size_t len) {
    return hal_i2c_read_reg(dev->i2c, dev->addr, reg, buf, len);
}

static hal_status_t write_reg(bme280_handle_t *dev, uint8_t reg, uint8_t val) {
    return hal_i2c_write_reg(dev->i2c, dev->addr, reg, &val, 1);
}

static hal_status_t load_calibration(bme280_handle_t *dev) {
    uint8_t buf[26];
    hal_status_t status = read_reg(dev, 0x88, buf, 26);
    if (status != HAL_OK) return status;

    dev->calib.dig_T1 = (uint16_t)(buf[1] << 8 | buf[0]);
    dev->calib.dig_T2 = (int16_t)(buf[3] << 8 | buf[2]);
    dev->calib.dig_T3 = (int16_t)(buf[5] << 8 | buf[4]);
    dev->calib.dig_P1 = (uint16_t)(buf[7] << 8 | buf[6]);
    dev->calib.dig_P2 = (int16_t)(buf[9] << 8 | buf[8]);
    dev->calib.dig_P3 = (int16_t)(buf[11] << 8 | buf[10]);
    dev->calib.dig_P4 = (int16_t)(buf[13] << 8 | buf[12]);
    dev->calib.dig_P5 = (int16_t)(buf[15] << 8 | buf[14]);
    dev->calib.dig_P6 = (int16_t)(buf[17] << 8 | buf[16]);
    dev->calib.dig_P7 = (int16_t)(buf[19] << 8 | buf[18]);
    dev->calib.dig_P8 = (int16_t)(buf[21] << 8 | buf[20]);
    dev->calib.dig_P9 = (int16_t)(buf[23] << 8 | buf[22]);

    uint8_t h1;
    status = read_reg(dev, 0xA1, &h1, 1);
    if (status != HAL_OK) return status;
    dev->calib.dig_H1 = h1;

    uint8_t hbuf[7];
    status = read_reg(dev, 0xE1, hbuf, 7);
    if (status != HAL_OK) return status;

    dev->calib.dig_H2 = (int16_t)(hbuf[1] << 8 | hbuf[0]);
    dev->calib.dig_H3 = hbuf[2];
    dev->calib.dig_H4 = (int16_t)((hbuf[3] << 4) | (hbuf[4] & 0x0F));
    dev->calib.dig_H5 = (int16_t)((hbuf[5] << 4) | ((hbuf[4] >> 4) & 0x0F));
    dev->calib.dig_H6 = (int8_t)hbuf[6];

    return HAL_OK;
}

/* Datasheet compensation formula — integer arithmetic, no floating point in the core */
static int32_t compensate_temp_int(bme280_handle_t *dev, int32_t adc_T) {
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
                    ((int32_t)dev->calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - (int32_t)dev->calib.dig_T1) *
                      ((adc_T >> 4) - (int32_t)dev->calib.dig_T1)) >> 12) *
                    (int32_t)dev->calib.dig_T3) >> 14;
    dev->t_fine = var1 + var2;
    return (dev->t_fine * 5 + 128) >> 8;  /* result in 0.01 degC */
}

static uint32_t compensate_pressure_int(bme280_handle_t *dev, int32_t adc_P) {
    int64_t var1 = (int64_t)dev->t_fine - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) +
           ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dev->calib.dig_P1) >> 33;
    if (var1 == 0) return 0;

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);
    return (uint32_t)p;  /* result in Pa * 256 */
}

static uint32_t compensate_humidity_int(bme280_handle_t *dev, int32_t adc_H) {
    int32_t v = dev->t_fine - 76800;
    v = (((((adc_H << 14) - ((int32_t)dev->calib.dig_H4 << 20) -
            ((int32_t)dev->calib.dig_H5 * v)) + 16384) >> 15) *
         (((((((v * (int32_t)dev->calib.dig_H6) >> 10) *
              (((v * (int32_t)dev->calib.dig_H3) >> 11) + 32768)) >> 10) +
            2097152) * (int32_t)dev->calib.dig_H2 + 8192) >> 14));
    v = v - (((((v >> 15) * (v >> 15)) >> 7) * (int32_t)dev->calib.dig_H1) >> 4);
    v = (v < 0) ? 0 : v;
    v = (v > 419430400) ? 419430400 : v;
    return (uint32_t)(v >> 12);  /* result in %RH * 1024 */
}

hal_status_t bme280_init(bme280_handle_t *dev, hal_i2c_handle_t *i2c, uint8_t addr) {
    if (!dev || !i2c) return HAL_INVALID_ARG;

    dev->i2c = i2c;
    dev->addr = addr;
    dev->initialized = false;
    dev->t_fine = 0;

    uint8_t chip_id = 0;
    hal_status_t status = read_reg(dev, BME280_REG_CHIP_ID, &chip_id, 1);
    if (status != HAL_OK) return status;
    if (chip_id != BME280_CHIP_ID) return HAL_ERROR;

    status = load_calibration(dev);
    if (status != HAL_OK) return status;

    /* Configure: humidity oversample x1 */
    status = write_reg(dev, BME280_REG_CTRL_HUM, BME280_OVERSAMPLE_1);
    if (status != HAL_OK) return status;

    /* Configure: temp+pressure oversample x1, forced mode */
    status = write_reg(dev, BME280_REG_CTRL_MEAS, (BME280_OVERSAMPLE_1 << 5) |
                                                   (BME280_OVERSAMPLE_1 << 2) | 0x01);
    if (status != HAL_OK) return status;

    dev->initialized = true;
    return HAL_OK;
}

hal_status_t bme280_read(bme280_handle_t *dev, bme280_data_t *data) {
    if (!dev || !data) return HAL_INVALID_ARG;
    if (!dev->initialized) return HAL_ERROR;

    /* Trigger forced measurement */
    hal_status_t status = write_reg(dev, BME280_REG_CTRL_MEAS,
                                    (BME280_OVERSAMPLE_1 << 5) |
                                    (BME280_OVERSAMPLE_1 << 2) | 0x01);
    if (status != HAL_OK) return status;

    uint8_t raw[8];
    status = read_reg(dev, BME280_REG_DATA_START, raw, 8);
    if (status != HAL_OK) return status;

    int32_t adc_P = (int32_t)((raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4));
    int32_t adc_T = (int32_t)((raw[3] << 12) | (raw[4] << 4) | (raw[5] >> 4));
    int32_t adc_H = (int32_t)((raw[6] << 8) | raw[7]);

    int32_t temp_raw = compensate_temp_int(dev, adc_T);
    data->temperature_c = temp_raw / 100.0f;

    uint32_t press_raw = compensate_pressure_int(dev, adc_P);
    data->pressure_pa = press_raw / 256.0f;

    uint32_t hum_raw = compensate_humidity_int(dev, adc_H);
    data->humidity_pct = hum_raw / 1024.0f;

    return HAL_OK;
}

hal_status_t bme280_soft_reset(bme280_handle_t *dev) {
    if (!dev) return HAL_INVALID_ARG;
    return write_reg(dev, BME280_REG_RESET, 0xB6);
}
