/**
 * @file data_processor.h
 * @brief Moving average filter and basic statistics on sensor data.
 */
#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include "utils/ring_buffer.h"

typedef struct {
    ring_buffer_t buffer;
    float         sum;
} moving_avg_t;

void  moving_avg_init(moving_avg_t *ma, size_t window_size);
void  moving_avg_push(moving_avg_t *ma, float value);
float moving_avg_get(const moving_avg_t *ma);
float moving_avg_get_min(const moving_avg_t *ma);
float moving_avg_get_max(const moving_avg_t *ma);

#endif
