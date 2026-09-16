#include "test_framework.h"
#include "drivers/adxl345.h"

extern void hal_spi_sim_set_reg(uint8_t reg, uint8_t value);
extern void hal_spi_sim_reset(void);

TEST_SUITE(test_adxl345) {
    hal_spi_sim_reset();

    /* Setup simulated registers */
    hal_spi_sim_set_reg(ADXL345_REG_DEVID, ADXL345_DEVICE_ID);
    hal_spi_sim_set_reg(ADXL345_REG_DATAX0, 0x10);
    hal_spi_sim_set_reg(ADXL345_REG_DATAX1, 0x00);  /* X = 16 */
    hal_spi_sim_set_reg(ADXL345_REG_DATAY0, 0xF0);
    hal_spi_sim_set_reg(ADXL345_REG_DATAY1, 0xFF);  /* Y = -16 */
    hal_spi_sim_set_reg(ADXL345_REG_DATAZ0, 0x00);
    hal_spi_sim_set_reg(ADXL345_REG_DATAZ1, 0x01);  /* Z = 256 */

    hal_spi_handle_t spi;
    hal_spi_init(&spi, 0, 1000000, SPI_MODE_3);

    adxl345_handle_t dev;

    /* Test init succeeds */
    TEST_ASSERT_EQ(adxl345_init(&dev, &spi, 5, ADXL345_RANGE_2G), HAL_OK);
    TEST_ASSERT(dev.initialized);

    /* Test init fails with wrong device ID */
    hal_spi_sim_set_reg(ADXL345_REG_DEVID, 0x00);
    adxl345_handle_t bad_dev;
    TEST_ASSERT(adxl345_init(&bad_dev, &spi, 5, ADXL345_RANGE_2G) != HAL_OK);
    hal_spi_sim_set_reg(ADXL345_REG_DEVID, ADXL345_DEVICE_ID);

    /* Test read returns expected values */
    adxl345_data_t data;
    TEST_ASSERT_EQ(adxl345_read(&dev, &data), HAL_OK);
    TEST_ASSERT_FLOAT_EQ(data.x_g, 16 * ADXL345_SCALE_FACTOR, 0.001f);
    TEST_ASSERT_FLOAT_EQ(data.y_g, -16 * ADXL345_SCALE_FACTOR, 0.001f);
    TEST_ASSERT_FLOAT_EQ(data.z_g, 256 * ADXL345_SCALE_FACTOR, 0.001f);

    /* Test null handling */
    TEST_ASSERT_EQ(adxl345_init(NULL, &spi, 5, ADXL345_RANGE_2G), HAL_INVALID_ARG);
    TEST_ASSERT_EQ(adxl345_read(&dev, NULL), HAL_INVALID_ARG);
}
