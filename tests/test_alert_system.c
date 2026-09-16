#include "test_framework.h"
#include "app/alert_system.h"
#include "hal/hal_gpio.h"

TEST_SUITE(test_alert_system) {
    alert_system_t sys;
    alert_system_init(&sys);

    /* Add high-temp alert: triggers above 35C, clears at 33C */
    TEST_ASSERT_EQ(alert_system_add(&sys, "temp", ALERT_ABOVE, 35.0f, 2.0f, 20), HAL_OK);

    /* Add low-temp alert: triggers below 0C, clears at 2C */
    TEST_ASSERT_EQ(alert_system_add(&sys, "temp", ALERT_BELOW, 0.0f, 2.0f, 21), HAL_OK);

    /* Normal temp — no alerts */
    alert_system_check(&sys, "temp", 25.0f);
    TEST_ASSERT(!alert_system_is_triggered(&sys, 0));
    TEST_ASSERT(!alert_system_is_triggered(&sys, 1));

    /* High temp — triggers alert 0 */
    alert_system_check(&sys, "temp", 36.0f);
    TEST_ASSERT(alert_system_is_triggered(&sys, 0));
    TEST_ASSERT_EQ(hal_gpio_read(20), GPIO_HIGH);

    /* Temp drops but still above hysteresis band — stays triggered */
    alert_system_check(&sys, "temp", 34.0f);
    TEST_ASSERT(alert_system_is_triggered(&sys, 0));

    /* Temp drops below hysteresis — clears */
    alert_system_check(&sys, "temp", 32.0f);
    TEST_ASSERT(!alert_system_is_triggered(&sys, 0));
    TEST_ASSERT_EQ(hal_gpio_read(20), GPIO_LOW);

    /* Low temp — triggers alert 1 */
    alert_system_check(&sys, "temp", -5.0f);
    TEST_ASSERT(alert_system_is_triggered(&sys, 1));
    TEST_ASSERT_EQ(hal_gpio_read(21), GPIO_HIGH);

    /* Different sensor name — does not affect temp alerts */
    alert_system_check(&sys, "humidity", 99.0f);
    TEST_ASSERT(alert_system_is_triggered(&sys, 1));  /* still triggered from temp */

    /* Test max alerts boundary */
    alert_system_t full_sys;
    alert_system_init(&full_sys);
    for (int i = 0; i < MAX_ALERTS; i++) {
        TEST_ASSERT_EQ(alert_system_add(&full_sys, "x", ALERT_ABOVE, 0, 0, (uint8_t)i), HAL_OK);
    }
    TEST_ASSERT(alert_system_add(&full_sys, "x", ALERT_ABOVE, 0, 0, 30) != HAL_OK);
}
