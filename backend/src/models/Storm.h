#ifndef STORM_H
#define STORM_H

#include <string>
#include <ctime>
#include "Coordinate.h"

class Storm {
public:
    std::string id;
    std::string name;
    std::string severity;
    std::string headline;
    std::string description;
    Coordinate position;  
    double windSpeed;
    double pressure;
    double velocity;
    double direction;
    time_t timestamp;
    
    Storm(); 
    std::string category() const;
    std::string getMapColor() const;
    Coordinate predictPosition(int hoursAhead) const;
    std::string toJSON() const;
};

#endif