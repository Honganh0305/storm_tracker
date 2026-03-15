#include "models/Coordinate.h"
#include <iostream>
#include <cmath>
#include <math.h>

Coordinate::Coordinate(double latitude, double longitude) 
    : lat(latitude), lon(longitude) {}

double Coordinate::calculate_distance(const Coordinate& other) const {

    const double R = 6371.0;
    double lat1 = lat * M_PI/ 180.0;
    double lon1 = lon * M_PI/180.0;
    double lat2 = other.lat *M_PI/180.0;
    double lon2 = other.lon * M_PI/180.0;


    double lat_distance = lat2 - lat1;
    double lon_distance = lon2 - lon1;

    double a = sin(lat_distance/2) * sin(lat_distance/2) + cos(lat1) * cos(lat2) * sin(lon_distance/2) * sin(lon_distance/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R*c;
}