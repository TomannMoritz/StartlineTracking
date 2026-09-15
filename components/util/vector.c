#include "math.h"

#include "vector.h"
#include "constants.h"

const uint8_t NORTH_OFFSET_DEG = 90;


//--------------------------------------------------
float positive_degree(float degree){
    float modulo = fmod(degree, DEG_FULL_CIRCLE);
    return sign_of_float(degree) >= 0 ? modulo : fmod(DEG_FULL_CIRCLE + modulo, DEG_FULL_CIRCLE);
}


//--------------------------------------------------
float compass_to_degree(float compass){
    compass = (compass - NORTH_OFFSET_DEG) * -1;
    compass = positive_degree(compass);
    return compass;
}


float degree_to_compass(float degree){
    degree = degree * -1 + NORTH_OFFSET_DEG;
    degree = positive_degree(degree);
    return degree;
}


//--------------------------------------------------
double degree_to_radiant(double degree){
    degree = positive_degree(degree);
    double radiant = degree * PI / DEG_HALF_CIRCLE;
    return radiant;
}


double radiant_to_degree(double radiant){
    double degree =  radiant * DEG_HALF_CIRCLE / PI;
    degree = positive_degree(degree);
    return degree;
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


//--------------------------------------------------
void scale_vector2(Vector2 *vector, float scalar){
    vector->x *= scalar;
    vector->y *= scalar;
}


void add_vector2(Vector2 *result, Vector2 *addition){
    result->x += addition->x;
    result->y += addition->y;
}


float vector2_length(Vector2 *vector){
    float quadratic_length = vector->x * vector->x + vector->y * vector->y;
    return sqrt(quadratic_length);
}
