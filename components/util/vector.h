#pragma once

typedef struct {
    float x;
    float y;
} Vector2;


//--------------------------------------------------
float compass_to_degree(float compass);
float degree_to_compass(float degree);


//--------------------------------------------------
double degree_to_radiant(double degree);
double radiant_to_degree(double radiant);


//--------------------------------------------------
Vector2 radiant_to_vector2(float radiant);
float vector2_to_radiant(Vector2 vector);


//--------------------------------------------------
Vector2 compass_to_vector2(float compass);
float vector2_to_compass(Vector2 vector);

