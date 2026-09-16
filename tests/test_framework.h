/**
 * @file test_framework.h
 * @brief Minimal unit test framework — no external dependencies.
 */
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <math.h>

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(expr) do { \
    tests_run++; \
    if (expr) { tests_passed++; } \
    else { tests_failed++; \
        printf("  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #expr); } \
} while(0)

#define TEST_ASSERT_EQ(a, b) TEST_ASSERT((a) == (b))

#define TEST_ASSERT_FLOAT_EQ(a, b, tol) TEST_ASSERT(fabs((double)(a) - (double)(b)) < (double)(tol))

#define TEST_SUITE(name) static void name(void)

#define RUN_SUITE(name) do { \
    printf("--- %s ---\n", #name); name(); \
} while(0)

#define TEST_REPORT() do { \
    printf("\n========================================\n"); \
    printf("Tests run: %d | Passed: %d | Failed: %d\n", tests_run, tests_passed, tests_failed); \
    printf("========================================\n"); \
} while(0)

#endif
