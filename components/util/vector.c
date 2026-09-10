#include "math.h"

#include "vector.h"
#include "constants.h"

const uint8_t NORTH_OFFSET_DEG = 90;


//--------------------------------------------------
float compass_to_degree(float compass){
    return (compass - NORTH_OFFSET_DEG) * -1;
}


float degree_to_compass(float degree){
    return degree * -1 + NORTH_OFFSET_DEG;
}


//--------------------------------------------------
double degree_to_radiant(double degree){
    return degree * PI / DEG_HALF_CIRCLE;
}


double radiant_to_degree(double radiant){
    return radiant * DEG_HALF_CIRCLE / PI;
}


//--------------------------------------------------
Vector2 radiant_to_vector2(float radiant){
   Vector2 vector = {
       .x = cos(radiant),
       .y = sin(radiant)
   };
   return vector;
}


float vector2_to_radiant(Vector2 vector){
    float radiant = acos(vector.x);
    if (sign_of_float(vector.y) >= 0){
        return radiant;
    }

    return PI - (radiant - PI);
}


//--------------------------------------------------
Vector2 compass_to_vector2(float compass){
    float degree = compass_to_degree(compass);
    float rad = degree_to_radiant(degree);
    Vector2 vector = radiant_to_vector2(rad);
    return vector;
}

float vector2_to_compass(Vector2 vector){
    float rad = vector2_to_radiant(vector);
    float degree = radiant_to_degree(rad);
    float compass = degree_to_compass(degree);
    return compass;
}
