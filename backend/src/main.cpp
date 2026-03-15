#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <ctime>

#include "core/StormTracker.h"
#include "models/Storm.h"
#include "models/Coordinate.h"
#include "models/Alert.h"
#include "api/WeatherAPI.h"

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);


    std::cout << "=== STORM TRACKING SYSTEM ===\n\n";

    StormTracker tracker;

    WeatherAPI api;
    auto nhcStorms = api.fetchGDACSStorms();
    auto nwsStorms = api.fetchNWSAlerts();

    std::vector<Storm> storms;
    storms.insert(storms.end(), nhcStorms.begin(), nhcStorms.end());
    storms.insert(storms.end(), nwsStorms.begin(), nwsStorms.end());

    std::string report;
    if (!storms.empty()) {
        std::cout << "Loaded " << nhcStorms.size() << " from NHC, " << nwsStorms.size() << " from NWS (" << storms.size() << " total storms).\n";

        for (const auto& s : storms) {
            tracker.updateStorm(s);
        }

        std::vector<Alert> nwsAlerts;
        time_t now = std::time(nullptr);
        for (const auto& s : nwsStorms) {
            std::string msg = s.headline.empty() ? s.name : s.headline;
            nwsAlerts.emplace_back(msg, s.severity.empty() ? "Unknown" : s.severity, now, s.description);
        }

        const std::string stormId = storms.front().id;
        report = tracker.generateReport(stormId, nwsAlerts);

        std::cout << "\nGenerated Report:\n" << report << "\n";
    } else {
        std::cout << "No storms returned (NHC or NWS). Writing empty payload so map still loads.\n";
        report = "{\"storm\":null,\"similar\":[],\"alerts\":[]}";
    }

    std::ofstream outFile("../frontend/public/data/storm_data.json");
    if (outFile.is_open()) {
        outFile << report;
        outFile.close();
        std::cout << "\n✓ Data written to ../frontend/public/data/storm_data.json\n";
    } else {
        std::cerr << "\n✗ Failed to write output file\n";
    }

    curl_global_cleanup();
    std::cout << "\n=== Program Complete ===\n";
    return 0;
}
