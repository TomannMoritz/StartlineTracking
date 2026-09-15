#pragma once
#include <stdint.h>


#define TEST_RANGE_INT(value, min, max) do {        \
    TEST_ASSERT_GREATER_OR_EQUAL_INT(min, value);   \
    TEST_ASSERT_LESS_OR_EQUAL_INT(max, value);      \
} while(0)


//--------------------------------------------------
extern const double PI;
extern const double DEG_HALF_CIRCLE;
extern const double DEG_FULL_CIRCLE;


//--------------------------------------------------
extern const uint32_t BASE_10_000;
extern const uint32_t BASE_1000;
extern const uint32_t BASE_100;
extern const uint8_t BASE_10;


//--------------------------------------------------
extern const uint32_t MIN_IN_DEG;
extern const uint32_t SEC_IN_MIN;
extern const uint32_t MIL_IN_SEC;

extern const uint32_t DEG_SCALE;
extern const uint32_t MIN_SCALE;
extern const uint32_t SEC_SCALE;


//--------------------------------------------------
float sign_of_float(float value);
