#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <iostream>
#include <string>
#include <vector>
#include "models/Storm.h"

class WeatherAPI {
    public:
    static std::vector<Storm> fetchNWSAlerts();
    static std::vector<Storm> fetchNHCStorms();
    static std::vector<Storm> fetchGDACSStorms();
    static std::string buildURL(const std::string& city, const std::string& apiKey);
    static std::string getCurrentWeather(const std::string& city, const std::string& apiKey);
    static double getTemperature(const std::string& json);
    static double getWindSpeed(const std::string& json);
    static double getPressure(const std::string& json);

    private:
    static constexpr const char* NHC_ARCHIVE_BASE =
        "https://www.nhc.noaa.gov/geojson/";
    static constexpr const char* NHC_5DAY_SUFFIX =
        "_5day.geojson";
    
};

#endif