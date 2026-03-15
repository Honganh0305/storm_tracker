#ifndef COORDINATE_H
#define COORDINATE_H

#include <iostream>

class Coordinate {
    public:
    double lat;
    double lon;

    Coordinate(double lat, double lon);
    double calculate_distance(const Coordinate& other) const;
};

#endif