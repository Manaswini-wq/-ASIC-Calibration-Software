/**
 * @file main.c
 * @brief Entry point — initializes hardware and runs the sensor acquisition loop.
 */
#include "hal/hal_i2c.h"
#include "hal/hal_spi.h"
#include "hal/hal_timer.h"
#include "app/sensor_manager.h"

/* Simulation helpers — declared in HAL .c files */
extern void hal_i2c_sim_add_device(uint8_t addr, const uint8_t *regs, size_t len);
extern void hal_spi_sim_set_reg(uint8_t reg, uint8_t value);

static void setup_simulated_sensors(void) {
    /* BME280 simulation: chip ID at register 0xD0 + fake raw data at 0xF7-0xFE */
    uint8_t bme_regs[256] = {0};
    bme_regs[0xD0] = 0x60;     /* chip ID */

    /* Calibration registers (0x88-0x9F): set dig_T1=27504, dig_T2=26435, dig_T3=-1000 */
    bme_regs[0x88] = 0x70; bme_regs[0x89] = 0x6B;  /* dig_T1 = 27504 */
    bme_regs[0x8A] = 0x43; bme_regs[0x8B] = 0x67;  /* dig_T2 = 26435 */
    bme_regs[0x8C] = 0x18; bme_regs[0x8D] = 0xFC;  /* dig_T3 = -1000 */

    /* Raw ADC data at 0xF7-0xFE (pressure, temp, humidity) */
    bme_regs[0xF7] = 0x50; bme_regs[0xF8] = 0x00; bme_regs[0xF9] = 0x00;
    bme_regs[0xFA] = 0x80; bme_regs[0xFB] = 0x00; bme_regs[0xFC] = 0x00;
    bme_regs[0xFD] = 0x6E; bme_regs[0xFE] = 0x00;

    hal_i2c_sim_add_device(0x76, bme_regs, 256);

    /* ADXL345 simulation: device ID at reg 0x00 + fake accel data at 0x32-0x37 */
    hal_spi_sim_set_reg(0x00, 0xE5);    /* device ID */
    hal_spi_sim_set_reg(0x32, 0x10);    /* X low byte */
    hal_spi_sim_set_reg(0x33, 0x00);    /* X high byte -> X = 16 LSB = 0.0624g */
    hal_spi_sim_set_reg(0x34, 0xF0);    /* Y low byte */
    hal_spi_sim_set_reg(0x35, 0xFF);    /* Y high byte -> Y = -16 LSB = -0.0624g */
    hal_spi_sim_set_reg(0x36, 0x00);    /* Z low byte */
    hal_spi_sim_set_reg(0x37, 0x01);    /* Z high byte -> Z = 256 LSB ~ 0.998g */
}

int main(void) {
    /* Init HAL */
    hal_timer_init();

    hal_i2c_handle_t i2c;
    hal_i2c_init(&i2c, 0, I2C_SPEED_FAST);

    hal_spi_handle_t spi;
    hal_spi_init(&spi, 0, 1000000, SPI_MODE_3);

    /* Setup simulated hardware registers */
    setup_simulated_sensors();

    /* Init sensor manager */
    sensor_manager_t mgr;
    if (sensor_manager_init(&mgr, &i2c, &spi, 5) != HAL_OK) {
        return 1;
    }

    /* Main loop — 20 samples at ~100ms interval */
    for (int i = 0; i < 20; i++) {
        sensor_manager_read_cycle(&mgr);

        if ((i + 1) % 10 == 0) {
            sensor_manager_print_report(&mgr);
        }
        hal_delay_ms(100);
    }

    sensor_manager_print_report(&mgr);
    return 0;
}
