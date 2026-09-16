#include "app/data_processor.h"
#include <float.h>

void moving_avg_init(moving_avg_t *ma, size_t window_size) {
    ring_buffer_init(&ma->buffer, window_size);
    ma->sum = 0.0f;
}

void moving_avg_push(moving_avg_t *ma, float value) {
    if (ring_buffer_full(&ma->buffer)) {
        float oldest;
        ring_buffer_pop(&ma->buffer, &oldest);
        ma->sum -= oldest;
    }
    ring_buffer_push(&ma->buffer, value);
    ma->sum += value;
}

float moving_avg_get(const moving_avg_t *ma) {
    size_t n = ring_buffer_count(&ma->buffer);
    return (n > 0) ? (ma->sum / (float)n) : 0.0f;
}

float moving_avg_get_min(const moving_avg_t *ma) {
    size_t n = ring_buffer_count(&ma->buffer);
    if (n == 0) return 0.0f;
    float min_val = FLT_MAX;
    for (size_t i = 0; i < n; i++) {
        float v = ring_buffer_peek(&ma->buffer, i);
        if (v < min_val) min_val = v;
    }
    return min_val;
}

float moving_avg_get_max(const moving_avg_t *ma) {
    size_t n = ring_buffer_count(&ma->buffer);
    if (n == 0) return 0.0f;
    float max_val = -FLT_MAX;
    for (size_t i = 0; i < n; i++) {
        float v = ring_buffer_peek(&ma->buffer, i);
        if (v > max_val) max_val = v;
    }
    return max_val;
}
