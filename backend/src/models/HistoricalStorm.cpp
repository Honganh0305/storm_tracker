#include "models/HistoricalStorm.h"
#include <iostream>
#include <cmath>

double HistoricalStorm::similarityScore(const Storm& current) const {
    if (maxWindSpeed == 0 || minPressure == 0) return 0.0;
    
    double wind_difference = std::abs(maxWindSpeed - current.windSpeed) / maxWindSpeed;
    double pressure_difference = std::abs(minPressure - current.pressure) / minPressure;

    double similarity_score = 1.0 - (wind_difference + pressure_difference) / 2.0;
    return similarity_score;
}

double HistoricalStorm::getMaxWindSpeed() const {
    return maxWindSpeed;
}

double HistoricalStorm::getMinPressure() const {
    return minPressure;
}

std::string HistoricalStorm::toJSON() const {
    return "{\"name\":\"" + name + "\",\"year\":" + std::to_string(year) + 
           ",\"maxWindSpeed\":" + std::to_string(maxWindSpeed) + 
           ",\"minPressure\":" + std::to_string(minPressure) + "}";
}