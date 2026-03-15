#ifndef HISTORICALSTORM_H
#define HISTORICALSTORM_H

#include <iostream>
#include <string>
#include "models/Coordinate.h"
#include "models/Storm.h"

class HistoricalStorm {
    public:
    std::string name;
    int year;
    double maxWindSpeed;
    double minPressure;
    int casualties;
    double damage;
    Coordinate maxIntensityLocation;

    HistoricalStorm() : year(0), maxWindSpeed(0), minPressure(0), casualties(0), damage(0), maxIntensityLocation(0.0, 0.0) {}
    
    double getMaxWindSpeed() const;
    double getMinPressure() const;
    double similarityScore(const Storm& current) const;
    std::string toJSON() const;
};
#endif