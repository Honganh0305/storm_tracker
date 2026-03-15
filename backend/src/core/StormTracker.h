#ifndef STORMTRACKER_H
#define STORMTRACKER_H

#include <iostream>
#include <string>
#include <vector>
#include "models/Storm.h"
#include "models/HistoricalStorm.h"
#include "models/Alert.h"
#include "utlis/HURDAT2Parser.h"

class StormTracker {
    private:
    std::vector<Storm> activeStorms;
    std::vector<HistoricalStorm> historicalData;
    std::vector<Alert> alerts;

    void initializeHistoricalData();
    void generateAlerts(const Storm& storm);

    public:
    StormTracker();
    void updateStorm(const Storm& storm);
    std::vector<HistoricalStorm> findSimilarStorms(const Storm& current, int limit = 3);
    std::string generateReport(const std::string& stormId);
    std::string generateReport(const std::string& stormId, const std::vector<Alert>& extraAlerts);
};

#endif