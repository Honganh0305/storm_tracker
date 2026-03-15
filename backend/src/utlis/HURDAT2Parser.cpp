#include "utlis/HURDAT2Parser.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>


std::vector<HURDAT2Parser::StormRecord> HURDAT2Parser::parse(const std::string& data) {
    std::vector<StormRecord> storms;
    std::istringstream stream(data);
    std::string line;
    StormRecord currentStorm;
    bool inStorm = false;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        if (line.find(',') != std::string::npos && line.length() < 50) {
            if (inStorm) {
                storms.push_back(currentStorm);
            }

            currentStorm = StormRecord();
            inStorm = true;

            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);

            currentStorm.id = line.substr(0, pos1);
            currentStorm.name = line.substr(pos1 + 1, pos2 - pos1 - 1);

            if (currentStorm.id.length() >= 8) {
                currentStorm.year = std::stoi(currentStorm.id.substr(4, 4));
            }

            currentStorm.name.erase(
                std::remove_if(currentStorm.name.begin(), currentStorm.name.end(), ::isspace),
                currentStorm.name.end()
            );
        }
        else if (inStorm) {
            std::istringstream lineStream(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(lineStream, token, ',')) {
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                tokens.push_back(token);
            }

            if (tokens.size() >= 8) {
                std::string latStr = tokens[4];
                double lat = 0;
                if (!latStr.empty() && latStr.length() > 1) {
                    lat = std::stod(latStr.substr(0, latStr.length() - 1));
                    if (latStr.back() == 'S') lat = -lat;
                }

                std::string lonStr = tokens[5];
                double lon = 0;
                if (!lonStr.empty() && lonStr.length() > 1) {
                    lon = std::stod(lonStr.substr(0, lonStr.length() - 1));
                    if (lonStr.back() == 'W') lon = -lon;
                }

                currentStorm.positions.push_back(Coordinate(lat, lon));

                if (!tokens[6].empty() && tokens[6] != "-999") {
                    double windKnots = std::stod(tokens[6]);
                    currentStorm.windSpeeds.push_back(windKnots * 1.852);
                }

                if (!tokens[7].empty() && tokens[7] != "-999") {
                    currentStorm.pressures.push_back(std::stod(tokens[7]));
                }
            }
        }
    }

    if (inStorm) {
        storms.push_back(currentStorm);
    }

    return storms;
}

HistoricalStorm HURDAT2Parser::convertToHistoricalStorm(const HURDAT2Parser::StormRecord& record) {
    HistoricalStorm list;
    list.name = record.name;
    list.year = record.year;

    list.maxWindSpeed = 0;
    for (double wind: record.windSpeeds) {
        if (wind > list.maxWindSpeed) list.maxWindSpeed = wind;
    }

    list.minPressure = 9999;
    for (double pressure: record.pressures) {
        if (pressure< list.minPressure) list.minPressure = pressure;
    }
    if (!record.positions.empty()) {
        list.maxIntensityLocation = record.positions[record.positions.size()/2];

    }
    list.casualties = 0;
    list.damage = 0;

    return list;

}
