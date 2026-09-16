/**
 * @file ring_buffer.h
 * @brief Lock-free ring buffer for sensor data samples.
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RING_BUFFER_MAX_SIZE 64

typedef struct {
    float    data[RING_BUFFER_MAX_SIZE];
    size_t   capacity;
    size_t   head;
    size_t   count;
} ring_buffer_t;

void   ring_buffer_init(ring_buffer_t *rb, size_t capacity);
bool   ring_buffer_push(ring_buffer_t *rb, float value);
bool   ring_buffer_pop(ring_buffer_t *rb, float *value);
size_t ring_buffer_count(const ring_buffer_t *rb);
bool   ring_buffer_full(const ring_buffer_t *rb);
bool   ring_buffer_empty(const ring_buffer_t *rb);
void   ring_buffer_clear(ring_buffer_t *rb);
float  ring_buffer_peek(const ring_buffer_t *rb, size_t index);

#endif
