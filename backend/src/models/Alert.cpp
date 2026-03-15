#include "models/Alert.h"
#include "api/WeatherAPI.h"
#include "models/Storm.h"
#include <vector>
#include <ctime>

Alert::Alert(const std::string& message, const std::string& severity, time_t timestamp,
             const std::string& descriptionOverride)
    : message(message), severity(severity),
      description(descriptionOverride.empty() ? message : descriptionOverride),
      timestamp(timestamp) {}

static void appendEscaped(std::string& out, const std::string& s) {
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
}

std::string Alert::toJSON() const {
    std::string out = "{\"message\":\"";
    appendEscaped(out, message);
    out += "\",\"severity\":\"" + severity + "\",\"description\":\"";
    appendEscaped(out, description);
    out += "\",\"timestamp\":" + std::to_string(timestamp) + "}";
    return out;
}

std::vector<Alert> Alert::getNWSAlerts() {
    std::vector<Storm> storms = WeatherAPI::fetchNWSAlerts();
    std::vector<Alert> alerts;

    if (storms.empty()) {
        alerts.emplace_back("No current alerts", "Info", std::time(nullptr));
    } else {
        for (const auto& storm : storms) {
            std::string msg = storm.name;
            if (!storm.severity.empty()) msg += " — " + storm.severity;
            std::string sev = storm.severity.empty() ? "Unknown" : storm.severity;
            alerts.emplace_back(msg, sev, std::time(nullptr));
        }
    }
    return alerts;
}