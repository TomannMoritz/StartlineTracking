#include "unity.h"


void app_main(void){
    UNITY_BEGIN();
    unity_run_tests_by_tag("[vector]", false);
    UNITY_END();
}
