#include "test_framework.h"
#include "drivers/bme280.h"

extern void hal_i2c_sim_add_device(uint8_t addr, const uint8_t *regs, size_t len);
extern void hal_i2c_sim_reset(void);

static void setup_bme280_device(void) {
    uint8_t regs[256] = {0};
    regs[0xD0] = BME280_CHIP_ID;

    /* Calibration data */
    regs[0x88] = 0x70; regs[0x89] = 0x6B;  /* dig_T1 = 27504 */
    regs[0x8A] = 0x43; regs[0x8B] = 0x67;  /* dig_T2 = 26435 */
    regs[0x8C] = 0x18; regs[0x8D] = 0xFC;  /* dig_T3 = -1000 */

    /* Raw sensor data */
    regs[0xF7] = 0x50; regs[0xF8] = 0x00; regs[0xF9] = 0x00;
    regs[0xFA] = 0x80; regs[0xFB] = 0x00; regs[0xFC] = 0x00;
    regs[0xFD] = 0x6E; regs[0xFE] = 0x00;

    hal_i2c_sim_add_device(BME280_I2C_ADDR_PRIMARY, regs, 256);
}

TEST_SUITE(test_bme280) {
    hal_i2c_sim_reset();
    setup_bme280_device();

    hal_i2c_handle_t i2c;
    hal_i2c_init(&i2c, 0, I2C_SPEED_FAST);

    bme280_handle_t dev;

    /* Test init succeeds with correct chip ID */
    TEST_ASSERT_EQ(bme280_init(&dev, &i2c, BME280_I2C_ADDR_PRIMARY), HAL_OK);
    TEST_ASSERT(dev.initialized);

    /* Test init fails with wrong address (no device) */
    bme280_handle_t bad_dev;
    TEST_ASSERT(bme280_init(&bad_dev, &i2c, 0x55) != HAL_OK);

    /* Test read returns valid data */
    bme280_data_t data;
    TEST_ASSERT_EQ(bme280_read(&dev, &data), HAL_OK);
    TEST_ASSERT(data.temperature_c > -40.0f && data.temperature_c < 85.0f);

    /* Test null pointer handling */
    TEST_ASSERT_EQ(bme280_init(NULL, &i2c, 0x76), HAL_INVALID_ARG);
    TEST_ASSERT_EQ(bme280_read(NULL, &data), HAL_INVALID_ARG);
    TEST_ASSERT_EQ(bme280_read(&dev, NULL), HAL_INVALID_ARG);
}
