/**
 * @file hal_timer.c
 * @brief Simulated timer — uses system clock on desktop.
 */
#include "hal/hal_timer.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

static uint32_t start_tick = 0;
static bool timer_inited = false;

hal_status_t hal_timer_init(void) {
#ifdef _WIN32
    start_tick = (uint32_t)GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    start_tick = (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
    timer_inited = true;
    return HAL_OK;
}

uint32_t hal_timer_get_tick_ms(void) {
    if (!timer_inited) return 0;
#ifdef _WIN32
    return (uint32_t)GetTickCount() - start_tick;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000) - start_tick;
#endif
}

void hal_delay_ms(uint32_t ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
