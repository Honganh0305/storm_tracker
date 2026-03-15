#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <iostream>
#include <string>
#include <curl/curl.h>

class HTTPClient {
    private:
    static size_t WriteCallback (void *contents, size_t size, size_t nmemb, void *userp);

    public:
    static std::string GET(const std::string& url);
    static std::string GET(const std::string& url, const std::vector<std::string>& headers);

};

#endif