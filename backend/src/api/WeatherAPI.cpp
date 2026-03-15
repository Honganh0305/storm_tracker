#include "api/WeatherAPI.h"
#include "utlis/HTTPClient.h"
#include "utlis/JSONBuilder.h"
#include "models/Storm.h"
#include "models/Coordinate.h"
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cmath>
#include <algorithm>

static size_t matchBrace(const std::string& str, size_t start) {
    if (start >= str.size() || str[start] != '{') return std::string::npos;
    int count = 1;
    for (size_t i = start + 1; i < str.size(); ++i) {
        if (str[i] == '{') count++;
        else if (str[i] == '}') count--;
        if (count == 0) return i;
    }
    return std::string::npos;
}

static bool isTestAlert(const std::string& eventName) {
    return eventName.find("Test") != std::string::npos;
}

static bool isStormRelated(const std::string& eventName) {
    static const std::vector<std::string> keywords = {
        "Hurricane", "Tropical Storm", "Cyclone", "Tornado", "Storm", "Typhoon"
    };
    for (const auto& k : keywords) if (eventName.find(k) != std::string::npos) return true;
    return false;
}

static bool isMarineRelated(const std::string& eventName) {
    static const std::vector<std::string> keywords = {
        "Small Craft", "Marine", "Coastal", "Flood"
    };
    for (const auto& k : keywords) if (eventName.find(k) != std::string::npos) return true;
    return false;
}

static void parseCoordinates(const std::string& feature, double& lat, double& lon) {
    lat = 0.0;
    lon = 0.0;
    std::string coords = JSONBuilder::extractObjectByKey(feature, "geometry");
    if (coords.empty()) return;
    size_t pos = coords.find("[");
    size_t end = coords.find("]");
    if (pos == std::string::npos || end == std::string::npos) return;
    std::string sub = coords.substr(pos + 1, end - pos - 1);
    std::replace(sub.begin(), sub.end(), ',', ' ');
    std::stringstream ss(sub);
    ss >> lon >> lat;
}

std::vector<Storm> WeatherAPI::fetchNWSAlerts() {
    std::vector<Storm> storms;

    const std::string url = "https://api.weather.gov/alerts/active";

    const std::vector<std::string> headers = {
        "Accept: application/geo+json",
        "User-Agent: StormTracker/1.0 (contact: you@example.com)"
    };

    const std::string response = HTTPClient::GET(url, headers);
    if (response.empty()) {
        std::cerr << "NWS request failed / empty response\n";
        return storms;
    }

    size_t featuresPos = response.find("\"features\"");
    if (featuresPos == std::string::npos) {
        std::cerr << "NWS response missing features\n";
        return storms;
    }

    size_t arrayStart = response.find('[', featuresPos);
    if (arrayStart == std::string::npos) {
        return storms;
    }

    size_t i = arrayStart + 1;

    while (i < response.size()) {

        while (i < response.size() &&
               (response[i] == ',' ||
                std::isspace(static_cast<unsigned char>(response[i])))) {
            ++i;
        }

        if (i >= response.size() || response[i] != '{') {
            break;
        }

        size_t end = matchBrace(response, i);
        if (end == std::string::npos) {
            break;
        }

        std::string feature = response.substr(i, end - i + 1);

        std::string props =
            JSONBuilder::extractObjectByKey(feature, "properties");

        if (props.empty()) {
            i = end + 1;
            continue;
        }

        std::string eventName =
            JSONBuilder::extractString(props, "event");

        if (eventName.empty() || isTestAlert(eventName)) {
            i = end + 1;
            continue;
        }

        if (!isStormRelated(eventName) &&
            !isMarineRelated(eventName)) {
            i = end + 1;
            continue;
        }

        Storm s;

        s.id = JSONBuilder::extractString(props, "id");
        s.name = eventName;
        s.severity = JSONBuilder::extractString(props, "severity");
        if (s.severity.empty()) s.severity = JSONBuilder::extractString(props, "urgency");
        if (s.severity.empty()) s.severity = "Unknown";
        s.headline = JSONBuilder::extractString(props, "headline");
        s.description = JSONBuilder::extractString(props, "description");

        double lat = NAN;
        double lon = NAN;

        if (feature.find("\"geometry\": null") == std::string::npos) {
            parseCoordinates(feature, lat, lon);
        }

        s.position = Coordinate(lat, lon);

        s.timestamp = std::time(nullptr);

        storms.push_back(std::move(s));

        i = end + 1;
    }

    return storms;
}

static const char* NHC_ARCHIVE_BASE = "https://www.nhc.noaa.gov/gis/forecast/archive/";
static const char* NHC_5DAY_SUFFIX = "_5day_latest.geojson";

static std::vector<std::string> buildNHCStormIds(int year) {
    std::vector<std::string> ids;
    for (int n = 1; n <= 30; n++) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "al%02d%d", n, year);
        ids.push_back(buf);
    }
    for (int n = 1; n <= 30; n++) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "ep%02d%d", n, year);
        ids.push_back(buf);
    }
    return ids;
}

std::vector<Storm> WeatherAPI::fetchNHCStorms() {
    std::vector<Storm> storms;

    time_t now = std::time(nullptr);
    struct tm* utc = std::gmtime(&now);
    int year = utc ? (utc->tm_year + 1900) : 2025;

    std::vector<std::string> headers = {
        "Accept: application/geo+json",
        "User-Agent: StormTracker/1.0 (https://github.com/storm-tracker)"
    };

    std::vector<int> yearsToTry = { year };
    if (year > 2020) yearsToTry.push_back(year - 1); 

    for (int y : yearsToTry) {
        if (!storms.empty()) break;
        std::vector<std::string> idsToTry = buildNHCStormIds(y);
        for (const std::string& stormId : idsToTry) {
            std::string url = std::string(NHC_ARCHIVE_BASE) + stormId + NHC_5DAY_SUFFIX;
            std::string response = HTTPClient::GET(url, headers);
            if (response.empty()) continue;
            if (response.find("\"type\"") == std::string::npos || response.find("Feature") == std::string::npos) continue;
        size_t featuresPos = response.find("\"features\"");
        if (featuresPos == std::string::npos) continue;
        size_t arrayStart = response.find('[', featuresPos);
        if (arrayStart == std::string::npos) continue;

        size_t i = arrayStart + 1;
        while (i < response.size() && (response[i] == ',' || std::isspace(static_cast<unsigned char>(response[i])))) i++;
        if (i >= response.size() || response[i] != '{') continue;
        size_t end = matchBrace(response, i);
        if (end == std::string::npos) continue;
        std::string feature = response.substr(i, end - i + 1);

        std::string props = JSONBuilder::extractObjectByKey(feature, "properties");
        if (props.empty()) continue;

        double lat = 0.0, lon = 0.0;
        parseCoordinates(feature, lat, lon);
        if (lat == 0.0 && lon == 0.0) continue;

        Storm s;
        s.id = stormId;
        s.name = JSONBuilder::extractString(props, "STORMNAME");
        if (s.name.empty()) s.name = JSONBuilder::extractString(props, "Name");
        if (s.name.empty()) s.name = stormId;

        s.position = Coordinate(lat, lon);
        s.windSpeed = JSONBuilder::extractDouble(props, "MAXWIND");
        if (s.windSpeed == 0.0) s.windSpeed = JSONBuilder::extractDouble(props, "WindSpeed");
        s.pressure = JSONBuilder::extractDouble(props, "MIN_PRESSURE");
        if (s.pressure == 0.0) s.pressure = JSONBuilder::extractDouble(props, "Pressure");
        s.velocity = 0.0;
        s.direction = 0.0;
        s.timestamp = now;

        storms.push_back(std::move(s));
        }
    }

    return storms;
}

std::vector<Storm> WeatherAPI::fetchGDACSStorms() {
    return fetchNHCStorms();
}

std::string WeatherAPI::buildURL(const std::string& city, const std::string& apiKey) {
    return "https://api.openweathermap.org/data/2.5/weather?q=" + city +"&appid=" + apiKey + "&units=metric";
}
std::string WeatherAPI::getCurrentWeather(const std::string& city, const std::string& apiKey) {
    std::string url = buildURL(city, apiKey);
    return HTTPClient::GET(url);
}

double WeatherAPI::getTemperature(const std::string& json) {
    return JSONBuilder::extractDouble(json, "temp");
}
double WeatherAPI::getWindSpeed(const std::string& json) {
    return JSONBuilder::extractDouble(json, "wind_speed");
}
double WeatherAPI::getPressure(const std::string& json) {
    return JSONBuilder::extractDouble(json, "pressure");
}