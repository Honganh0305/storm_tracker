#ifndef JSONBUILDER_H
#define JSONBUILDER_H

#include <iostream>
#include <string>
#include <vector>

class JSONBuilder {
    public:
        static std::string extractString(const std::string& json, const std::string& key);
        static double extractDouble(const std::string& json, const std::string& key);
    

        static std::string extractObjectByKey(const std::string& json, const std::string& key);
        static std::string buildObject(const std::string& stormJSON,
                                       const std::vector<std::string>& similarJSON,
                                       const std::vector<std::string>& alertsJSON);
    };

#endif 