/**
 * @file alert_system.h
 * @brief Threshold-based alerting with hysteresis and GPIO output.
 */
#ifndef ALERT_SYSTEM_H
#define ALERT_SYSTEM_H

#include "hal/hal_gpio.h"
#include "utils/logger.h"
#include <stdbool.h>

#define MAX_ALERTS 8

typedef enum {
    ALERT_ABOVE,    /* trigger when value > threshold */
    ALERT_BELOW     /* trigger when value < threshold */
} alert_type_t;

typedef struct {
    const char   *name;
    alert_type_t  type;
    float         threshold;
    float         hysteresis;
    uint8_t       gpio_pin;
    bool          active;
    bool          triggered;
} alert_config_t;

typedef struct {
    alert_config_t alerts[MAX_ALERTS];
    size_t         count;
} alert_system_t;

void         alert_system_init(alert_system_t *sys);
hal_status_t alert_system_add(alert_system_t *sys, const char *name,
                              alert_type_t type, float threshold,
                              float hysteresis, uint8_t gpio_pin);
void         alert_system_check(alert_system_t *sys, const char *sensor_name,
                                float value);
bool         alert_system_is_triggered(const alert_system_t *sys, size_t index);

#endif
