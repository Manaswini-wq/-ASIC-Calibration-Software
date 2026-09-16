#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "hal_types.h"

hal_status_t hal_timer_init(void);
uint32_t     hal_timer_get_tick_ms(void);
void         hal_delay_ms(uint32_t ms);

#endif
