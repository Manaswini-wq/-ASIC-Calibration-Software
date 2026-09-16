#include "app/alert_system.h"
#include <string.h>

void alert_system_init(alert_system_t *sys) {
    memset(sys, 0, sizeof(*sys));
}

hal_status_t alert_system_add(alert_system_t *sys, const char *name,
                              alert_type_t type, float threshold,
                              float hysteresis, uint8_t gpio_pin) {
    if (sys->count >= MAX_ALERTS) return HAL_ERROR;

    alert_config_t *a = &sys->alerts[sys->count];
    a->name = name;
    a->type = type;
    a->threshold = threshold;
    a->hysteresis = hysteresis;
    a->gpio_pin = gpio_pin;
    a->active = true;
    a->triggered = false;

    hal_gpio_init(gpio_pin, GPIO_DIR_OUTPUT);
    hal_gpio_write(gpio_pin, GPIO_LOW);

    sys->count++;
    return HAL_OK;
}

void alert_system_check(alert_system_t *sys, const char *sensor_name, float value) {
    for (size_t i = 0; i < sys->count; i++) {
        alert_config_t *a = &sys->alerts[i];
        if (!a->active || strcmp(a->name, sensor_name) != 0) continue;

        bool should_trigger = false;
        if (a->type == ALERT_ABOVE) {
            if (!a->triggered && value > a->threshold)
                should_trigger = true;
            else if (a->triggered && value < (a->threshold - a->hysteresis))
                should_trigger = false;
            else
                continue;
        } else {
            if (!a->triggered && value < a->threshold)
                should_trigger = true;
            else if (a->triggered && value > (a->threshold + a->hysteresis))
                should_trigger = false;
            else
                continue;
        }

        a->triggered = should_trigger;
        hal_gpio_write(a->gpio_pin, should_trigger ? GPIO_HIGH : GPIO_LOW);

        if (should_trigger) {
            LOG_W("ALERT", "%s triggered: value=%.2f threshold=%.2f",
                  a->name, (double)value, (double)a->threshold);
        } else {
            LOG_I("ALERT", "%s cleared: value=%.2f", a->name, (double)value);
        }
    }
}

bool alert_system_is_triggered(const alert_system_t *sys, size_t index) {
    if (index >= sys->count) return false;
    return sys->alerts[index].triggered;
}
