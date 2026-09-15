#include "unity.h"

#include "constants.h"
#include "vector.h"


// NOTE: use only positive degree/radiant as angle representation
TEST_CASE("convert between degree and compass representation", "[vector]"){
    uint32_t step_size = 45;
    for(int32_t degree = -DEG_FULL_CIRCLE; degree <= DEG_FULL_CIRCLE; degree += step_size){
        float compass = degree_to_compass(degree);
        float result = compass_to_degree(compass);

        int target_degree = (int)positive_degree(degree);
        TEST_RANGE_INT((int32_t) result, target_degree - 1, target_degree + 1);
    }
}


TEST_CASE("convert between degree and radiant representation", "[vector]"){
    uint32_t step_size = 45;
    for(int32_t degree = -DEG_FULL_CIRCLE; degree <= DEG_FULL_CIRCLE; degree += step_size){
        float radiants = degree_to_radiant(degree);
        float result = radiant_to_degree(radiants);

        int target_degree = (int)positive_degree(degree);
        TEST_RANGE_INT((int32_t) result, target_degree - 1, target_degree + 1);
    }
}


TEST_CASE("convert between degree and vector2 representation", "[vector]"){
    uint32_t step_size = 45;
    for(int32_t degree = -DEG_FULL_CIRCLE; degree <= DEG_FULL_CIRCLE; degree += step_size){
        float radiants = degree_to_radiant(degree);
        Vector2 vector = radiant_to_vector2(radiants);
        float result_rad = vector2_to_radiant(vector);
        float result_deg = radiant_to_degree(result_rad);

        int target_degree = (int)positive_degree(degree);
        TEST_RANGE_INT((int32_t) result_deg, target_degree - 1, target_degree + 1);
    }
}


TEST_CASE("convert between compass degree and vector2 representation", "[vector]"){
    uint32_t step_size = 45;
    for(int32_t compass_degree = -DEG_FULL_CIRCLE; compass_degree <= DEG_FULL_CIRCLE; compass_degree += step_size){
        Vector2 vector = compass_to_vector2((float)compass_degree);
        float result_degree = vector2_to_compass(vector);

        int target_degree = (int)positive_degree(compass_degree);
        TEST_RANGE_INT((int32_t) result_degree, target_degree - 1, target_degree + 1);
    }
}

