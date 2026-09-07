#include "math.h"

#include "gps_util.h"


const uint32_t RADIUS_EARTH_KM = 6371;
const double PI = 3.14159265359;
const double DEG_HALF_CIRCLE = 180.0;


//--------------------------------------------------
// Reference: https://en.wikipedia.org/wiki/Haversine_formula
//--------------------------------------------------
// Haversine formula:
//      d = r * arcsin((v)^0.5) * 2
//      v = sin^2((phi_2 - phi_1) / 2) + cos(phi_2) * cos(phi_1) * sin^2((lambda_2 - lambda_1) / 2)
//      
//      NOTE:
//          d is the distance on the surface of the sphere
//          v is the squared distance/length of the vector between both coordinates
//          phi: Latitude angle
//          lambda: Longitude angle
//--------------------------------------------------
static double haversine(double phi_1, double lambda_1, double phi_2, double lambda_2){
    double diff_latitude = phi_2 - phi_1;
    double diff_longitude = lambda_2 - lambda_1;

    double squared_vector = sin(diff_latitude / 2) * sin(diff_latitude / 2);
    squared_vector += cos(phi_2) * cos(phi_1) * sin(diff_longitude / 2) * sin(diff_longitude / 2);

    double half_length = sqrt(squared_vector);
    double half_angle = asin(half_length);
    double full_angle = half_angle * 2.0;
    return full_angle;
}


static double deg_to_rad(double deg){
    double rad = deg * PI / DEG_HALF_CIRCLE;

    return rad;
}


//--------------------------------------------------
double calculate_haversine_distance(Coordinate *position_1, Coordinate *position_2){
    double lat_num_1 = (double)latitude_to_number(&position_1->latitude) / DEG_SCALE;
    double long_num_1 = (double)longitude_to_number(&position_1->longitude) / DEG_SCALE;

    double lat_num_2 = (double)latitude_to_number(&position_2->latitude) / DEG_SCALE;
    double long_num_2 = (double)longitude_to_number(&position_2->longitude) / DEG_SCALE;

    double phi_2 = deg_to_rad(lat_num_2);
    double phi_1 = deg_to_rad(lat_num_1);

    double lambda_2 = deg_to_rad(long_num_2);
    double lambda_1 = deg_to_rad(long_num_1);

    double result = haversine(phi_1, lambda_1, phi_2, lambda_2);
    return result * RADIUS_EARTH_KM;
}
