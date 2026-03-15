#include "utlis/JSONBuilder.h"
#include <cctype>
#include <cstdlib>
#include <stdexcept>

static inline void skipWS(const std::string& s, size_t& i) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) i++;
}

static inline size_t findKeyPos(const std::string& json, const std::string& key) {
    const std::string quotedKey = "\"" + key + "\"";
    size_t k = json.find(quotedKey);
    if (k == std::string::npos) return std::string::npos;
    k += quotedKey.size();
    size_t colon = json.find(':', k);
    return colon;
}

static inline bool isNumChar(char c) {
    return (c == '-') || (c == '+') || (c == '.') ||
           (c >= '0' && c <= '9') || (c == 'e') || (c == 'E');
}

static size_t matchClosing(const std::string& s, size_t openPos, char openCh, char closeCh) {
    if (openPos >= s.size() || s[openPos] != openCh) return std::string::npos;

    int depth = 0;
    bool inString = false;

    for (size_t i = openPos; i < s.size(); i++) {
        char c = s[i];

        if (c == '"' && (i == 0 || s[i - 1] != '\\')) {
            inString = !inString;
        }
        if (inString) continue;

        if (c == openCh) depth++;
        else if (c == closeCh) {
            depth--;
            if (depth == 0) return i;
        }
    }
    return std::string::npos;
}

std::string JSONBuilder::extractString(const std::string& json, const std::string& key) {
    size_t colon = findKeyPos(json, key);
    if (colon == std::string::npos) return "";

    size_t pos = colon + 1;
    skipWS(json, pos);

    if (pos + 4 <= json.size() && json.compare(pos, 4, "null") == 0) return "";

    if (pos >= json.size() || json[pos] != '"') return "";
    pos++; 

    std::string out;
    out.reserve(32);

    for (size_t i = pos; i < json.size(); i++) {
        char c = json[i];
        if (c == '"' && json[i - 1] != '\\') {
            return out;
        }
        if (c == '\\' && i + 1 < json.size()) {
            char n = json[i + 1];
            switch (n) {
                case '"': out.push_back('"'); i++; continue;
                case '\\': out.push_back('\\'); i++; continue;
                case '/': out.push_back('/'); i++; continue;
                case 'b': out.push_back('\b'); i++; continue;
                case 'f': out.push_back('\f'); i++; continue;
                case 'n': out.push_back('\n'); i++; continue;
                case 'r': out.push_back('\r'); i++; continue;
                case 't': out.push_back('\t'); i++; continue;
                default: 
                    out.push_back(n);
                    i++;
                    continue;
            }
        }
        out.push_back(c);
    }
    return "";
}

double JSONBuilder::extractDouble(const std::string& json, const std::string& key) {
    size_t colon = findKeyPos(json, key);
    if (colon == std::string::npos) return 0.0;

    size_t pos = colon + 1;
    skipWS(json, pos);

    if (pos + 4 <= json.size() && json.compare(pos, 4, "null") == 0) return 0.0;

    bool quoted = (pos < json.size() && json[pos] == '"');
    if (quoted) pos++;

    size_t start = pos;
    while (pos < json.size() && isNumChar(json[pos])) pos++;

    if (pos == start) return 0.0;

    if (quoted) {
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) pos++;
        if (pos >= json.size() || json[pos] != '"') return 0.0;
    }

    try {
        return std::stod(json.substr(start, pos - start));
    } catch (...) {
        return 0.0;
    }
}

std::string JSONBuilder::extractObjectByKey(const std::string& json, const std::string& key) {
    size_t colon = findKeyPos(json, key);
    if (colon == std::string::npos) return "";

    size_t pos = colon + 1;
    skipWS(json, pos);

    if (pos >= json.size()) return "";

    if (json[pos] == '{') {
        size_t end = matchClosing(json, pos, '{', '}');
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos + 1);
    }
    if (json[pos] == '[') {
        size_t end = matchClosing(json, pos, '[', ']');
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos + 1);
    }
    return "";
}

std::string JSONBuilder::buildObject(const std::string& stormJSON,
                                     const std::vector<std::string>& similarJSON,
                                     const std::vector<std::string>& alertsJSON) {
    std::string result = "{\"storm\":" + stormJSON + ",\"similar\":[";
    for (size_t i = 0; i < similarJSON.size(); i++) {
        if (i > 0) result += ",";
        result += similarJSON[i];
    }
    result += "],\"alerts\":[";
    for (size_t i = 0; i < alertsJSON.size(); i++) {
        if (i > 0) result += ",";
        result += alertsJSON[i];
    }
    result += "]}";
    return result;
}
