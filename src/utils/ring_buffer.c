#include "utils/ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb, size_t capacity) {
    rb->capacity = (capacity <= RING_BUFFER_MAX_SIZE) ? capacity : RING_BUFFER_MAX_SIZE;
    rb->head = 0;
    rb->count = 0;
}

bool ring_buffer_push(ring_buffer_t *rb, float value) {
    size_t idx = (rb->head + rb->count) % rb->capacity;
    rb->data[idx] = value;

    if (rb->count < rb->capacity) {
        rb->count++;
    } else {
        rb->head = (rb->head + 1) % rb->capacity;  /* overwrite oldest */
    }
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, float *value) {
    if (rb->count == 0) return false;
    *value = rb->data[rb->head];
    rb->head = (rb->head + 1) % rb->capacity;
    rb->count--;
    return true;
}

size_t ring_buffer_count(const ring_buffer_t *rb) { return rb->count; }
bool ring_buffer_full(const ring_buffer_t *rb) { return rb->count >= rb->capacity; }
bool ring_buffer_empty(const ring_buffer_t *rb) { return rb->count == 0; }

void ring_buffer_clear(ring_buffer_t *rb) {
    rb->head = 0;
    rb->count = 0;
}

float ring_buffer_peek(const ring_buffer_t *rb, size_t index) {
    if (index >= rb->count) return 0.0f;
    return rb->data[(rb->head + index) % rb->capacity];
}
