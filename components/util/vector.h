#pragma once

typedef struct {
    float x;
    float y;
} Vector2;


//--------------------------------------------------
float positive_degree(float degree);


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


//--------------------------------------------------
void scale_vector2(Vector2 *vector, float scalar);
void add_vector2(Vector2 *result, Vector2 *addition);
float vector2_length(Vector2 *vector);
