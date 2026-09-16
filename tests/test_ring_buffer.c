#include "test_framework.h"
#include "utils/ring_buffer.h"

TEST_SUITE(test_ring_buffer) {
    ring_buffer_t rb;

    /* Test init */
    ring_buffer_init(&rb, 4);
    TEST_ASSERT(ring_buffer_empty(&rb));
    TEST_ASSERT_EQ(ring_buffer_count(&rb), 0);

    /* Test push and pop */
    ring_buffer_push(&rb, 1.0f);
    ring_buffer_push(&rb, 2.0f);
    ring_buffer_push(&rb, 3.0f);
    TEST_ASSERT_EQ(ring_buffer_count(&rb), 3);

    float val;
    ring_buffer_pop(&rb, &val);
    TEST_ASSERT_FLOAT_EQ(val, 1.0f, 0.001f);
    TEST_ASSERT_EQ(ring_buffer_count(&rb), 2);

    /* Test overflow — overwrites oldest */
    ring_buffer_clear(&rb);
    ring_buffer_push(&rb, 10.0f);
    ring_buffer_push(&rb, 20.0f);
    ring_buffer_push(&rb, 30.0f);
    ring_buffer_push(&rb, 40.0f);
    TEST_ASSERT(ring_buffer_full(&rb));
    ring_buffer_push(&rb, 50.0f);  /* overwrites 10.0 */
    ring_buffer_pop(&rb, &val);
    TEST_ASSERT_FLOAT_EQ(val, 20.0f, 0.001f);

    /* Test peek */
    ring_buffer_clear(&rb);
    ring_buffer_push(&rb, 100.0f);
    ring_buffer_push(&rb, 200.0f);
    TEST_ASSERT_FLOAT_EQ(ring_buffer_peek(&rb, 0), 100.0f, 0.001f);
    TEST_ASSERT_FLOAT_EQ(ring_buffer_peek(&rb, 1), 200.0f, 0.001f);
}
