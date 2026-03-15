#ifndef ALERT_H
#define ALERT_H

#include <iostream>
#include <time.h>
#include <string>

class Alert {
public:
    std::string message;
    std::string severity;
    std::string description;
    time_t timestamp;

    Alert() = default;
    Alert(const std::string& message, const std::string& severity, time_t timestamp,
          const std::string& descriptionOverride = "");

    std::string toJSON() const;
    static std::vector<Alert> getNWSAlerts();
};
#endif