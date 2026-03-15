#include "core/StormTracker.h"
#include "utlis/HTTPClient.h"
#include "utlis/HURDAT2Parser.h"
#include "models/Coordinate.h"
#include "utlis/JSONBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>

StormTracker::StormTracker() {
    initializeHistoricalData();
}
void StormTracker::initializeHistoricalData() {
    std::cout << "Fetching historical storm data from NOAA HURDAT2..." << std::endl;

    std::string url =
        "https://www.nhc.noaa.gov/data/hurdat/hurdat2-1851-2023-051124.txt";
    std::string hurdat2Data = HTTPClient::GET(url);

    std::cout << "Parsing HURDAT2 data..." << std::endl;
    auto stormRecords = HURDAT2Parser::parse(hurdat2Data);

    for (const auto& record : stormRecords) {
        if (record.year >= 1990) {
            HistoricalStorm hist =
                HURDAT2Parser::convertToHistoricalStorm(record);

            if (hist.maxWindSpeed > 178) {
                historicalData.push_back(hist);
            }
        }
    }

    std::cout << "Loaded " << historicalData.size()
              << " major hurricanes for comparison" << std::endl;
}


void StormTracker::generateAlerts(const Storm& storm) {
    alerts.clear();

    std::vector<std::pair<std::string, Coordinate>> areas = {
        {"Miami, FL", Coordinate(25.76, -80.19)},
        {"New Orleans, LA", Coordinate(29.95, -90.07)},
        {"Houston, TX", Coordinate(29.76, -95.36)},
        {"Tampa, FL", Coordinate(27.95, -82.45)}
    };

    time_t now = time(nullptr);
    Coordinate pred24 = storm.predictPosition(24);

    for (const auto& area : areas) {
        double dist = pred24.calculate_distance(area.second);

        if (dist < 100) {
            std::string severity = (dist < 50) ? "WARNING" : "WATCH";
            std::string msg =
                storm.name + " (" + storm.category() +
                ") predicted within " + std::to_string((int)dist) +
                "km in 24 hours";

            alerts.push_back(Alert(msg, severity, now));
        }
    }
}

void StormTracker::updateStorm(const Storm& storm) {
    activeStorms.erase(
        std::remove_if(activeStorms.begin(), activeStorms.end(),
            [&](const Storm& s) { return s.id == storm.id; }),
        activeStorms.end()
    );

    activeStorms.push_back(storm);
    generateAlerts(storm);
}

std::vector<HistoricalStorm>
StormTracker::findSimilarStorms(const Storm& current, int limit) {
    std::vector<std::pair<HistoricalStorm, double>> scored;

    for (const auto& hist : historicalData) {
        scored.push_back({hist, hist.similarityScore(current)});
    }

    std::sort(scored.begin(), scored.end(),
        [](const std::pair<HistoricalStorm, double>& a, const std::pair<HistoricalStorm, double>& b) {
            return a.second > b.second;
        });

    std::vector<HistoricalStorm> result;
    for (int i = 0; i < std::min(limit, (int)scored.size()); i++) {
        result.push_back(scored[i].first);
    }
    return result;
}

std::string StormTracker::generateReport(const std::string& stormId) {
    return generateReport(stormId, {});
}

std::string StormTracker::generateReport(const std::string& stormId, const std::vector<Alert>& extraAlerts) {
    auto it = std::find_if(
        activeStorms.begin(), activeStorms.end(),
        [&](const Storm& s) { return s.id == stormId; });

    if (it == activeStorms.end()) {
        return "{\"error\":\"Storm not found\"}";
    }

    Storm& storm = *it;
    auto similar = findSimilarStorms(storm);

    std::vector<std::string> similarJSON;
    for (const auto& s : similar) {
        double similarity = s.similarityScore(storm);
        std::string json = s.toJSON();
        json.pop_back(); // Remove '}'
        json += ",\"similarity\":" + std::to_string(similarity * 100.0) + "}";
        similarJSON.push_back(json);
    }

    std::vector<std::string> alertsJSON;
    for (const auto& alert : alerts) {
        alertsJSON.push_back(alert.toJSON());
    }
    for (const auto& alert : extraAlerts) {
        alertsJSON.push_back(alert.toJSON());
    }

    return JSONBuilder::buildObject(storm.toJSON(), similarJSON, alertsJSON);
}
