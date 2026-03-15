#ifndef HURDAT2PARSER_H
#define HURDAT2PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include "models/Coordinate.h"
#include "models/HistoricalStorm.h"

class HURDAT2Parser {
    public:
    struct StormRecord {
        std::string id;
        std::string name;
        int year;
        std::vector<double> windSpeeds;
        std::vector<double> pressures;
        std::vector<Coordinate> positions;
    };

    static std::vector<StormRecord> parse(const std::string& data);
    static HistoricalStorm convertToHistoricalStorm(const StormRecord& record);
};

#endif