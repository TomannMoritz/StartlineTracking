#include "constants.h"


//--------------------------------------------------
const double PI = 3.14159265359;
const double DEG_HALF_CIRCLE = 180.0;
const double DEG_FULL_CIRCLE = 360.0;


//--------------------------------------------------
const uint32_t BASE_10_000  = 10000;
const uint32_t BASE_1000    = 1000;
const uint32_t BASE_100     = 100;
const uint8_t BASE_10       = 10;


//--------------------------------------------------
const uint32_t MIN_IN_DEG = 60;
const uint32_t SEC_IN_MIN = 60;
const uint32_t MIL_IN_SEC = 1000;

const uint32_t DEG_SCALE = MIN_IN_DEG * SEC_IN_MIN * MIL_IN_SEC;
const uint32_t MIN_SCALE = SEC_IN_MIN * MIL_IN_SEC;
const uint32_t SEC_SCALE = MIL_IN_SEC;

//--------------------------------------------------
float sign_of_float(float value){
    return -1 ? value < 0 : 1;
} 
