#include "models/Storm.h"
#include <iostream>
#include <cmath>
#include <vector>

static std::string jsonNumber(double x) {
    if (std::isfinite(x)) return std::to_string(x);
    return "null";
}

Storm::Storm()
    : id(""), name(""), severity(""), position(0.0, 0.0), windSpeed(0.0), pressure(0.0),
      velocity(0.0), direction(0.0), timestamp(0) {
}

std::string Storm::category() const {
    if (windSpeed < 119) return "Tropical Storm";
    if (windSpeed < 154) return "Category 1";
    if (windSpeed < 177) return "Category 2";
    if (windSpeed < 209) return "Category 3";
    if (windSpeed < 251) return "Category 4";
    return "Category 5";
}

Coordinate Storm::predictPosition(int hoursAhead) const {
    const double R = 6371.0; 
    double distKm = velocity * hoursAhead;
    double dirRad = direction * M_PI / 180.0;
    
    double lat1 = position.lat * M_PI / 180.0;
    double lon1 = position.lon * M_PI / 180.0;
    
    double lat2 = asin(sin(lat1) * cos(distKm/R) + 
                      cos(lat1) * sin(distKm/R) * cos(dirRad));
    double lon2 = lon1 + atan2(sin(dirRad) * sin(distKm/R) * cos(lat1),
                               cos(distKm/R) - sin(lat1) * sin(lat2));
    
    return Coordinate(lat2 * 180.0 / M_PI, lon2 * 180.0 / M_PI);
}

std::string Storm::getMapColor() const {
    if (windSpeed < 119) return "#00FFFF"; 
    if (windSpeed < 154) return "#FFFF00"; 
    if (windSpeed < 177) return "#FFA500"; 
    if (windSpeed < 209) return "#FF6347";  
    if (windSpeed < 251) return "#FF0000"; 
    return "#8B0000";                       
}

std::string Storm::toJSON() const {
    std::vector<std::string> pathPoints;
    for (int hours : {6, 12, 24, 48, 72}) {
        Coordinate pred = predictPosition(hours);
        std::string pathPoint = "{\"hours\":" + std::to_string(hours) +
                               ",\"position\":{\"lat\":" + jsonNumber(pred.lat) +
                               ",\"lon\":" + jsonNumber(pred.lon) + "}}";
        pathPoints.push_back(pathPoint);
    }

    std::string predictedPath = "[";
    for (size_t i = 0; i < pathPoints.size(); i++) {
        predictedPath += pathPoints[i];
        if (i < pathPoints.size() - 1) predictedPath += ",";
    }
    predictedPath += "]";

    std::string sevEscaped;
    for (char c : severity) {
        if (c == '"') sevEscaped += "\\\"";
        else if (c == '\\') sevEscaped += "\\\\";
        else sevEscaped += c;
    }
    return "{\"id\":\"" + id +
           "\",\"name\":\"" + name +
           "\",\"severity\":\"" + sevEscaped + "\"" +
           ",\"position\":{\"lat\":" + jsonNumber(position.lat) +
           ",\"lon\":" + jsonNumber(position.lon) +
           "},\"windSpeed\":" + jsonNumber(windSpeed) +
           ",\"pressure\":" + jsonNumber(pressure) +
           ",\"velocity\":" + jsonNumber(velocity) +
           ",\"direction\":" + jsonNumber(direction) +
           ",\"category\":\"" + category() +
           "\",\"color\":\"" + getMapColor() +
           "\",\"predictedPath\":" + predictedPath + "}";
}